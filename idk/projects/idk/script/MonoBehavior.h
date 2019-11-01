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
		ManagedObject script_data;

		string_view TypeName() const;
		MonoObject* EmplaceBehavior(string_view type);
		void DisposeMonoObject();

		ManagedObject& GetObject() { return script_data; };
		const ManagedObject& GetObject() const { return script_data; };

		void Awake();
		void Start();
		void FixedUpdate();
		void Update();
		void UpdateCoroutines();

		Behavior() = default;
		Behavior(const Behavior&);
		Behavior(Behavior&& rhs) = default;
		Behavior& operator=(const Behavior&);
		Behavior& operator=(Behavior&& rhs) = default;
		~Behavior() = default;
	private:
		string        _serialized;
		bool          _awake{};
		bool          _started{};
	};
}