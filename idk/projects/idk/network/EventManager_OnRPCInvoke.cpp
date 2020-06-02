#include "stdafx.h"
#include "EventManager.h"

#include <prefab/Prefab.h>
#include <scene/SceneManager.h>
#include <core/GameObject.inl>
#include <common/Transform.h>

#include <network/ConnectionManager.inl>
#include <network/TestMessage.h>
#include <network/NetworkSystem.inl>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/ElectronTransformView.h>
#include <network/EventLoadLevelMessage.h>
#include <network/EventInstantiatePrefabMessage.h>
#include <network/EventTransferOwnershipMessage.h>
#include <network/EventInvokeRPCMessage.h>

#include <mono/jit/jit.h>
#include <script/ScriptSystem.h>
#include <script/MonoWrapperEnvironment.h>
#include <script/MonoBehavior.h>
#include <script/ManagedObj.inl>

namespace idk
{
	static void InvokeRPC(Host sender, const EventInvokeRPCMessage::Data& payload);

	void EventManager::SendRPC(Handle<ElectronView> ev, string_view method_name, span<vector<unsigned char>> buffer)
	{
		LOG_TO(LogPool::NETWORK, "Sending RPC for %d and method %s to client %d", ev->network_id, method_name.data(), (int) connection_manager->GetConnectedHost());
		EventInvokeRPCMessage::Data prototype_message;
		{
			prototype_message.invoke_on_id = ev->network_id;
			//strncpy_s(prototype_message.method_name, method_name.data(), method_name.size());
			prototype_message.method_id = Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment().GetRpcIdFor(method_name);
			prototype_message.param_count = (int)buffer.size();
			prototype_message.param_buffer.clear();
			prototype_message.param_buffer.resize(prototype_message.param_count);
			for (unsigned i = 0; i < prototype_message.param_count; ++i)
			{
				prototype_message.param_buffer[i].size = (int)buffer[i].size();
				prototype_message.param_buffer[i].buffer = buffer[i];
			};
		}
		
		connection_manager->CreateAndSendMessage<EventInvokeRPCMessage>(GameChannel::RELIABLE, [&](EventInvokeRPCMessage& msg)
		{
			msg.payload = prototype_message;
		});
	}

	void EventManager::BroadcastRPC(Handle<ElectronView> ev, RPCTarget target, string_view method_name, span<vector<unsigned char>> buffer)
	{
		constexpr std::string_view RPCLookup[] = {"All", "Others", "Server"};
		LOG_TO(LogPool::NETWORK, "Broadcasting RPC for %d and method %s to the %s", ev->network_id, method_name.data(), RPCLookup[static_cast<int>(target)].data());
		EventInvokeRPCMessage::Data prototype_message;
		{
			prototype_message.invoke_on_id = ev->network_id;
			//strncpy_s(prototype_message.method_name, method_name.data(), method_name.size());
			prototype_message.method_id = Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment().GetRpcIdFor(method_name);
			prototype_message.param_count = (int)buffer.size();
			prototype_message.param_buffer.clear();
			prototype_message.param_buffer.resize(prototype_message.param_count);
			for (unsigned i = 0; i < prototype_message.param_count; ++i)
			{
				prototype_message.param_buffer[i].size = (int)buffer[i].size();
				prototype_message.param_buffer[i].buffer = buffer[i];
			};
		}

		if (Core::GetSystem<NetworkSystem>().IsHost()) // am the host
		{
			if (target == RPCTarget::All || target == RPCTarget::Server)
				InvokeRPC(Host::SERVER, prototype_message);

			if (target == RPCTarget::All || target == RPCTarget::Others)
			{
				Core::GetSystem<NetworkSystem>().BroadcastMessage<EventInvokeRPCMessage>(GameChannel::RELIABLE, [&](EventInvokeRPCMessage& msg)
					{
						msg.payload = prototype_message;
					});
			}
		}
		else // not the host
		{
			// send the message to the server
			if (target == RPCTarget::Server)
			{
				auto server_connection = Core::GetSystem<NetworkSystem>().GetConnectionTo(Host::SERVER);
				server_connection->CreateAndSendMessage<EventInvokeRPCMessage>(GameChannel::RELIABLE, [&](EventInvokeRPCMessage& msg)
					{
						msg.payload = prototype_message;
					});
			}
		}

	}

	void EventManager::OnInvokeRPCMessage(EventInvokeRPCMessage& raw_msg)
	{
		const auto invoke_rpc = 
		[	connection_manager = this->connection_manager, 
			payload = raw_msg.payload]
		(RscHandle<Scene>)
		{
			InvokeRPC(connection_manager->GetConnectedHost(), payload);
		};

		if (scene_changing)
			Core::GetSystem<SceneManager>().OnSceneChange.Listen(invoke_rpc, 1);
		else
			invoke_rpc({});
	}

	struct ElectronMessageInfo
	{
		unsigned long long view_handle;
		int host;
		unsigned short frameStamp;
	};

	static void InvokeRPC(Host sender, const EventInvokeRPCMessage::Data& msg)
	{
		auto method_name = Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment().GetFunctionNameFromRpcId(msg.method_id);

		LOG_TO(LogPool::NETWORK, "Received RPC for object %d to invoke %s", msg.invoke_on_id, method_name.data());
		const auto view = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(msg.invoke_on_id);
		if (view)
		{
			const auto& env = Core::GetSystem<mono::ScriptSystem>().Environment();
			// get monobehaviors
			auto thunk = env.Type("ElectronView")->GetThunk("Reserialize", 1);
			const auto arr_sz = msg.param_buffer.size();
			auto arr = mono_array_new(mono_domain_get(), mono_get_array_class(), arr_sz);

			for (int i = 0; i < msg.param_buffer.size(); ++i)
			{
				auto& param = msg.param_buffer[i].buffer;
				auto subarr = mono_array_new(mono_domain_get(), mono_get_byte_class(), param.size());

				for (int j = 0; j < param.size(); ++j)
					mono_array_set(subarr, unsigned char, j, param[j]);
				mono_array_setref(arr, i, subarr);
			}

			using Reserialize = MonoArray * (*)(MonoArray*, MonoObject**);

			MonoObject* exc;
			auto params = static_cast<Reserialize>(thunk->get())(arr, &exc);

			auto message_info = env.Type("ElectronMessageInfo")->Construct();
			message_info.Assign("fromID", static_cast<int>(sender));
			message_info.Assign("view_handle", view.id);

			if (exc)
			{
				Core::GetSystem<mono::ScriptSystem>().HandleException(exc);
			}

			IDK_ASSERT_MSG(params, "parameters could not be instantiated");
			unsigned successes{};
			for (auto& elem : view->GetGameObject()->GetComponents<mono::Behavior>())
			{
				if (elem->InvokeRPC(method_name.data(), params, message_info.Raw()))
					++successes;
			}
			if (successes == 0)
				LOG_TO(LogPool::NETWORK, "Called nonexistent RPC (%s)", method_name.data());
			else
				LOG_TO(LogPool::NETWORK, "Executed RPC (%s) %d times ", method_name.data(), successes);
		}
	}
}
