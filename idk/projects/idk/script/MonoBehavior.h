#pragma once

#include <mono/jit/jit.h>

#include <core/Component.h>
#include <script/ManagedObj.h>

namespace idk::mono
{
	struct MonoBehaviorData;

	class Behavior 
		: public Component<Behavior, false>
	{
	public:
		bool enabled = true;
		MonoValue behavior_values;

		MonoValue& Retrieve();
		void Submit();

		string_view TypeName() const;
		MonoObject* EmplaceBehavior(string_view type);

		ManagedObject& GetObject() { return _obj; };
		const ManagedObject& GetObject() const { return _obj; };

		void DisposeMonoObject();

		void SerializeFromString(string_view type, string_view behavior_values);

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