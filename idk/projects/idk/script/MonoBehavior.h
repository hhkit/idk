#pragma once
#include <core/Component.h>
#include <mono/jit/jit.h>

namespace idk::mono
{
	struct MonoBehaviorData;

	class Behavior 
		: public Component<Behavior>
	{
	public:
		Behavior();
		Behavior(Behavior&& rhs);
		Behavior& operator=(Behavior&& rhs);
		~Behavior();

		const std::string& RescueMonoObject();
		void RestoreMonoObject();

		void EmplaceBehavior(string_view type);
		void DisposeMonoObject();

		void SerializeFromString(string_view type, string_view serialized);
		MonoObject* GetMonoObject();
		MonoBehaviorData* GetData();

		//void EditorUpdate() override;
		//void DebugDraw() override;
		//void Start() override;
		//void Update() override;
		//void Stop() override;

		void UpdateCoroutines();
	private:
		friend class MonoSystem;

		MonoObject*                      _obj {}; // c# object
		MonoBehaviorData*               _data {}; // data to idk metadata for type
		hash_table<string, MonoMethod*> _methods;
		string                          _type;
		string                          _serialized;

		bool initialized                      {};
		uint32_t _gc_handle                   {};
	};
}