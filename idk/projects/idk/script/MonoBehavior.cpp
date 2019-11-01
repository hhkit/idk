#include "stdafx.h"
#include "MonoBehavior.h"
#include <serialize/text.h>

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoFunctionInvoker.h>
namespace idk::mono
{
	string_view Behavior::TypeName() const
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
		return nullptr;
	}

	void Behavior::DisposeMonoObject()
	{
		script_data = {};
	}

	void Behavior::Awake()
	{
		if (!_awake && script_data)
		{
			_awake = true;
			auto method = script_data.Type()->GetThunk("Awake");
			if (method)
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::Start()
	{
		if (!_started && script_data)
		{
			_started = true;
			auto method = script_data.Type()->GetThunk("Start");
			if (method)
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::FixedUpdate()
	{
		if (enabled && script_data)
		{
			auto method = script_data.Type()->GetThunk("FixedUpdate");
			if (method)
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::Update()
	{
		if (enabled && script_data)
		{
			auto method = script_data.Type()->GetThunk("Update");
			if (method)
				method->Invoke(script_data.Raw());
		}
	}

	void Behavior::UpdateCoroutines()
	{
		if (enabled && script_data)
		{
			auto t = Core::GetSystem<ScriptSystem>().Environment().Type("MonoBehavior");
			
			auto method = t->GetMethod("UpdateCoroutines");
			if (method.index() == 0)
				std::get<ManagedThunk>(method).Invoke(script_data.Raw());
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
	
}