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

	void EventManager::BroadcastRPC(Handle<ElectronView> ev, RPCTarget target, string_view method_name, span<vector<unsigned char>> buffer)
	{
		LOG_TO(LogPool::NETWORK, "Broadcasting RPC for %d and method %s", ev->network_id, method_name.data());
		EventInvokeRPCMessage::Data prototype_message;
		{
			prototype_message.invoke_on_id = ev->network_id;
			strncpy_s(prototype_message.method_name, method_name.data(), method_name.size());
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
		InvokeRPC(connection_manager->GetConnectedHost(), raw_msg.payload);
	}

	static void InvokeRPC(Host sender, const EventInvokeRPCMessage::Data& msg)
	{
		LOG_TO(LogPool::NETWORK, "Received RPC for object %d to invoke %s", msg.invoke_on_id, msg.method_name);
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

			using Reserialize = MonoArray * (*)(MonoArray*, MonoException**);

			MonoException* exc;
			auto params = static_cast<Reserialize>(thunk->get())(arr, &exc);

			auto message_info_type = env.Type("ElectronMessageInfo");
			auto message_info = message_info_type->Construct();
			int sender_as_int = static_cast<int>(sender);
			message_info.Assign("fromID", sender_as_int);
			message_info.Assign("view_handle", view.id);
			if (exc)
			{
				auto idk = Core::GetSystem<mono::ScriptSystem>().Environment().Type("IDK");
				auto method = std::get<1>(idk->GetMethod("PrintException", 1));
				void* args[] = { exc, 0 };
				mono_runtime_invoke(method, nullptr, args, nullptr);
			}

			IDK_ASSERT_MSG(params, "parameters could not be instantiated");
			for (auto& elem : view->GetGameObject()->GetComponents<mono::Behavior>())
				elem->InvokeRPC(msg.method_name, params, message_info.Raw());
		}
	}
}
