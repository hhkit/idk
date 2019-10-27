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

		string_view TypeName() const;
		MonoObject* EmplaceBehavior(string_view type);
		void DisposeMonoObject();

		ManagedObject& GetObject() { return _obj; };
		const ManagedObject& GetObject() const { return _obj; };

		void Awake();
		void Start();
		void FixedUpdate();
		void Update();
		void UpdateCoroutines();

		Behavior() = default;
		//Behavior(const Behavior&);
		//Behavior& operator=(const Behavior&);
	private:
		ManagedObject _obj;
		string        _serialized;
		bool          _awake{};
		bool          _started{};
	};
}