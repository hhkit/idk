#include "stdafx.h"
#include "MonoBehavior.h"
#include "ManagedObj.inl"
#include <serialize/text.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoFunctionInvoker.h>

#include <concurrent_queue.h>
namespace idk::mono
{
	string Behavior::TypeName() const
	{
		return script_data.TypeName();
	}

	MonoObject* Behavior::EmplaceBehavior(string_view type)
	{
		auto* env = &Core::GetSystem<ScriptSystem>().ScriptEnvironment();
		if (env)
			if (const auto monotype = env->Type(type))
			{
				script_data = monotype->Construct();
				script_data.Assign("handle", GetHandle().id);
				_awake = false;
				_started = false;
				return script_data.Raw();
			}
		script_data = ManagedObject{ type };
		return nullptr;
	}

	void Behavior::DisposeMonoObject()
	{
		script_data = {};
	}

	void Behavior::FireMessage(string_view msg, [[maybe_unused]] void* args[])
	{
		if (script_data)
		{
			if (auto thunk = script_data.Type()->GetThunk(msg))
				thunk->Invoke(script_data.Raw()); // handle args?
		}
	}

	void Behavior::Awake()
	{
		if (!_awake && script_data)
		{
			_awake = true;
			if (auto method = script_data.Type()->GetThunk("Awake"))
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::Start()
	{
		if (!_started && script_data)
		{
			_started = true;
			if (auto method = script_data.Type()->GetThunk("Start"))
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::FixedUpdate()
	{
		if (enabled && script_data)
		{
			if (auto method = script_data.Type()->GetThunk("FixedUpdate"))
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::Update()
	{
		if (enabled && script_data)
		{
			if (auto method = script_data.Type()->GetThunk("Update"))
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::UpdateCoroutines()
	{
		if (enabled && script_data)
		{
			if (auto thunk = script_data.Type()->GetThunk("UpdateCoroutines"))
				thunk->Invoke(script_data.Raw());
		}
	}

	void Behavior::InvokeRPC(string_view rpc, MonoArray* params, MonoObject* message_info)
	{
		auto rpc_method = script_data.Type()->GetRPC(rpc);
		if (rpc_method)
		{
			if (rpc_method.has_message_info_arg)
			{
				auto param_length = mono_array_length(params);
				auto arr = mono_array_new(mono_domain_get(), mono_get_object_class(), param_length + 1);
				
				mono_array_memcpy_refs(arr, 0, params, 0, param_length);

				mono_array_setref(arr, param_length - 1, message_info);
				mono_runtime_invoke_array(rpc_method.method, script_data.Raw(), arr, nullptr);
			}
			else
			{
				mono_runtime_invoke_array(rpc_method.method, script_data.Raw(), params, nullptr);
			}
		}
	}
	
	Behavior::Behavior(const Behavior& rhs)
		: enabled{ rhs.enabled }
		, script_data{}
	{
		if (const auto* type = rhs.script_data.Type())
		{
			auto clone_method = type->GetMethod("Clone");
			IDK_ASSERT(clone_method.index() != 2);
			script_data = ManagedObject{ Invoke(clone_method, rhs.script_data.Raw()) };
			IDK_ASSERT(script_data.Raw());
		}
	}

	Behavior& Behavior::operator=(const Behavior& rhs)
	{
		if (const auto* type = rhs.script_data.Type())
		{
			auto clone_method = type->GetMethod("Clone");
			auto new_obj = ManagedObject{ Invoke(clone_method, rhs.script_data) };
			enabled = rhs.enabled;
			script_data = new_obj;
			IDK_ASSERT(script_data.Raw());
		}
		else
		{
			DisposeMonoObject();
			enabled = rhs.enabled;
		}

		return *this;
	}

	Behavior::~Behavior()
	{
		if (const auto* type = script_data.Type())
		{
			if (auto thunk = type->GetThunk("OnDestroy"))
				thunk->Invoke(script_data.Raw());
		}
	}
	
}