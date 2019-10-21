#include "stdafx.h"
#include "MonoBehavior.h"
#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
namespace idk::mono
{
	string_view Behavior::RescueMonoObject()
	{
		return _serialized;
	}

	void Behavior::RestoreMonoObject()
	{
	}

	string_view Behavior::TypeName() const
	{
		return _obj.TypeName();
	}

	MonoObject* Behavior::EmplaceBehavior(string_view type)
	{
		auto monotype = Core::GetSystem<ScriptSystem>().ScriptEnvironment().Type(type);
		if (monotype)
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