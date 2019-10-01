#pragma once
#include <core/Component.h>
#include <mono/jit/jit.h>

namespace idk
{
	struct MonoBehaviorData;

	class MonoBehavior 
		: public Component<MonoBehavior>
	{
		friend class MonoSystem;
		//MonoObject* _obj = nullptr; // c# object
		MonoBehaviorData* _data = nullptr;
		hash_table<string, MonoMethod*> _methods;
		string _type;
		string _serialized;

		uint32_t _gc_handle{};
	public:
		MonoBehavior();
		MonoBehavior(MonoBehavior&& rhs);
		MonoBehavior&& operator=(MonoBehavior&& rhs);
		~MonoBehavior();

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
	};
}