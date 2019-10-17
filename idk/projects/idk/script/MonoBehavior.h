#pragma once

#include <mono/jit/jit.h>

#include <core/Component.h>
#include <script/ManagedObj.h>

namespace idk::mono
{
	struct MonoBehaviorData;

	class Behavior 
		: public Component<Behavior>
	{
	public:
		bool enabled = true;

		string_view RescueMonoObject();
		void        RestoreMonoObject();

		MonoObject* EmplaceBehavior(string_view type);
		MonoObject* GetObject();
		void DisposeMonoObject();

		void SerializeFromString(string_view type, string_view serialized);

		void Awake();
		void Start();
		void FixedUpdate();
		void Update();
		//void Stop() override;

		void UpdateCoroutines();
	private:
		ManagedObject _obj;
		string        _serialized;
		bool          _awake{};
		bool          _started{};
	};
}