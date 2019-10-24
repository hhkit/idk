#include "stdafx.h"
#include "MonoBehavior.h"
#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
namespace idk::mono
{
	MonoValue& Behavior::Retrieve()
	{
		if (_obj)
			behavior_values = _obj.Value();
		return behavior_values;
	}

	void Behavior::Submit()
	{
		if (EmplaceBehavior(behavior_values.name))
			_obj.Value(span<const MonoValue>{ std::get<vector<MonoValue>>(behavior_values.data) });
	}

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
			auto method = _obj.Type()->GetMethod("Awake");
			if (method.index() == 0)
				std::get<ManagedThunk>(method).Invoke(_obj.Raw());
		}
	}

	void Behavior::Start()
	{
		if (!_started && _obj)
		{
			_started = true;
			auto method = _obj.Type()->GetMethod("Start");
			if (method.index() == 0)
				std::get<ManagedThunk>(method).Invoke(_obj.Raw());
		}
	}

	void Behavior::FixedUpdate()
	{
		if (enabled && _obj)
		{
			auto method = _obj.Type()->GetMethod("FixedUpdate");
			if (method.index() == 0)
				std::get<ManagedThunk>(method).Invoke(_obj.Raw());
		}
	}

	void Behavior::Update()
	{
		if (enabled && _obj)
		{
			auto method = _obj.Type()->GetMethod("Update");
			if (method.index() == 0)
				std::get<ManagedThunk>(method).Invoke(_obj.Raw());
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

}