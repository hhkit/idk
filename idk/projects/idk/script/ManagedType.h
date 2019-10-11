#pragma once
#include <idk.h>

#include <mono/jit/jit.h>

#include <script/ManagedThunk.h>

namespace idk::mono
{
	class ManagedObject;

	class ManagedType
	{
	public:
		ManagedType(MonoClass* type);

		template<typename ... Args> 
		ManagedObject Construct(Args&&...);

		void Assign(string_view field, MonoObject* obj);
		bool CacheThunk(string_view method_name, int param_count = 0);
		std::variant <ManagedThunk, MonoMethod*, std::nullopt_t> GetMethod(string_view method_name, int param_count = 0) const;
	private:
		MonoClass* type{};
		hash_table<string, ManagedThunk> thunks;

		MonoMethod* FindMethod(string_view method_name, int param_count) const;
	};
}