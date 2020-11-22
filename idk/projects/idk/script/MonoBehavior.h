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

		string TypeName() const;
		MonoObject* EmplaceBehavior(string_view type);
		void DisposeMonoObject();

		ManagedObject& GetObject() { return script_data; };
		const ManagedObject& GetObject() const { return script_data; };

		template<typename ... Args> // TODO: Research perfect forwarding
		void FireMessage(string_view msg, Args&&... args)
		{
			if (script_data)
			{
				if (auto thunk = script_data.Type()->GetThunk(msg))
					thunk->Invoke(script_data.Raw(), std::forward<Args>(args)...);
			}
		}

		void Awake();
		void Start();
		void FixedUpdate();
		void Update();
		void UpdateCoroutines();

		bool InvokeRPC(string_view rpc, MonoArray* params, void* electron_message);

		Behavior() = default;
		Behavior(const Behavior&);
		Behavior(Behavior&& rhs) = default;
		Behavior& operator=(const Behavior&);
		Behavior& operator=(Behavior&& rhs) = default;
		~Behavior();
	private:
		string        _serialized;
		bool          _awake{};
		bool          _started{};
	};
}