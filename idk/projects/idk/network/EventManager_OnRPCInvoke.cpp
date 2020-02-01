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

namespace idk
{

	void EventManager::BroadcastRPC(Handle<ElectronView> ev, string_view method_name, span<vector<unsigned char>> buffer)
	{
		LOG_TO(LogPool::NETWORK, "Broadcasting RPC for %d and method %s", ev->network_id, method_name.data());
		Core::GetSystem<NetworkSystem>().BroadcastMessage<EventInvokeRPCMessage>(GameChannel::RELIABLE, [&](EventInvokeRPCMessage& msg)
			{
				msg.invoke_on_id = ev->network_id;
				strncpy_s(msg.method_name, method_name.data(), method_name.size());
				msg.param_count = (int)buffer.size();
				msg.param_buffer.clear();
				msg.param_buffer.resize(msg.param_count);
				for (int i = 0; i < msg.param_count; ++i)
				{
					msg.param_buffer[i].size = buffer[i].size();
					msg.param_buffer[i].buffer = buffer[i];
				}
			});

	}

	void EventManager::OnInvokeRPCMessage(EventInvokeRPCMessage* msg)
	{
		LOG_TO(LogPool::NETWORK, "Received RPC for object %d to invoke %s", msg->invoke_on_id, msg->method_name);
		auto view = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(msg->invoke_on_id);
		if (view)
		{
			// get monobehaviors
			auto thunk = Core::GetSystem<mono::ScriptSystem>().Environment().Type("ElectronView")->GetThunk("Reserialize",1);
			auto arr = mono_array_new(mono_domain_get(), mono_get_array_class(), msg->param_buffer.size());
			
			for (int i = 0; i < msg->param_buffer.size(); ++i)
			{
				auto& param = msg->param_buffer[i].buffer;
				auto subarr = mono_array_new(mono_domain_get(), mono_get_byte_class(), param.size());

				for (int j = 0; j < param.size(); ++j)
					mono_array_set(subarr, unsigned char, j, param[j]);
				mono_array_setref(arr, i, subarr);
			}

			using Reserialize = MonoArray * (*)(MonoArray*, MonoException**);

			MonoException* exc;
			auto params = static_cast<Reserialize>(thunk->get())(arr, &exc);
			if (exc)
			{
				auto idk = Core::GetSystem<mono::ScriptSystem>().Environment().Type("IDK");
				auto method = std::get<1>(idk->GetMethod("PrintException", 1));
				void* args[] = { exc, 0 };
				mono_runtime_invoke(method, nullptr, args, nullptr); 
			}
			
			IDK_ASSERT_MSG(params, "parameters could not be instantiated");
			for (auto& elem : view->GetGameObject()->GetComponents<mono::Behavior>())
				elem->InvokeRPC(msg->method_name, params);
		}
	}
}
