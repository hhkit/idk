#include "stdafx.h"
#include "MonoBehavior.h"
#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
#include <script/MonoFunctionInvoker.h>
namespace idk::mono
{
	string_view Behavior::TypeName() const
	{
		return _obj.TypeName();
	}

	MonoObject* Behavior::EmplaceBehavior(string_view type)
	{
		auto* env = &Core::GetSystem<ScriptSystem>().ScriptEnvironment();
		if (env)
			if (const auto monotype = env->Type(type))
			{
				_obj = monotype->Construct();
				_obj.Assign("handle", GetHandle().id);
				_awake = false;
				_started = false;
				return _obj.Raw();
			}
		return nullptr;
	}

	void Behavior::DisposeMonoObject()
	{
		_obj = {};
	}

	void Behavior::Awake()
	{
		if (!_awake && _obj)
		{
			_awake = true;
			auto method = _obj.Type()->GetThunk("Awake");
			if (method)
				method->Invoke(_obj.Raw());
		}
	}

	void Behavior::Start()
	{
		if (!_started && _obj)
		{
			_started = true;
			auto method = _obj.Type()->GetThunk("Start");
			if (method)
				method->Invoke(_obj.Raw());
		}
	}

	void Behavior::FixedUpdate()
	{
		if (enabled && _obj)
		{
			auto method = _obj.Type()->GetThunk("FixedUpdate");
			if (method)
				method->Invoke(_obj.Raw());
		}
	}

	void Behavior::Update()
	{
		if (enabled && _obj)
		{
			auto method = _obj.Type()->GetThunk("Update");
			if (method)
				method->Invoke(_obj.Raw());
		}
	}

	void Behavior::UpdateCoroutines()
	{
		if (enabled && _obj)
		{
			auto t = Core::GetSystem<ScriptSystem>().Environment().Type("MonoBehavior");
			
			auto method = t->GetMethod("UpdateCoroutines");
			if (method.index() == 0)
				std::get<ManagedThunk>(method).Invoke(_obj.Raw());
		}
	}
	/*
	Behavior::Behavior(const Behavior& rhs)
		: enabled{ rhs.enabled }
		, _obj{}
	{
		if (const auto* type = rhs._obj.Type())
		{
			auto clone_method = type->GetMethod("Clone");
			IDK_ASSERT(clone_method.index() != 2);
			_obj = ManagedObject{ Invoke(clone_method, rhs._obj) };
		}
	}

	Behavior& Behavior::operator=(const Behavior& rhs)
	{
		if (const auto* type = rhs._obj.Type())
		{
			auto clone_method = type->GetMethod("Clone");
			auto new_obj = ManagedObject{ Invoke(clone_method, rhs._obj) };
			DisposeMonoObject();
			enabled = rhs.enabled;
			_obj = new_obj;
		}
		else
		{
			DisposeMonoObject();
			enabled = rhs.enabled;
		}

		return *this;
		// TODO: insert return statement here
	}
	*/

}