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
		explicit ManagedType(MonoClass* type);

		MonoClass* Raw() const;

		template<typename ... Args>
		MonoObject* ConstructTemporary(Args&& ...);

		template<typename ... Args> 
		ManagedObject Construct(Args&&...);
		bool CacheThunk(string_view method_name, int param_count = 0);
		std::variant <ManagedThunk, MonoMethod*, std::nullopt_t> GetMethod(string_view method_name, int param_count = 0) const;
	private:
		MonoClass* type{};
		hash_table<string, ManagedThunk> thunks;

		MonoMethod* FindMethod(string_view method_name, int param_count) const;
	};

	template<typename ...Args>
	inline MonoObject* ManagedType::ConstructTemporary(Args&& ...)
	{
		const auto domain = mono_domain_get();
		auto obj = mono_object_new(domain, type);

		// construct 
		if constexpr (sizeof...(Args) == 0)
			mono_runtime_object_init(obj);
		else
		{
			//GetMethod()
		}
		return obj;
	}

	template<typename ...Args>
	inline ManagedObject ManagedType::Construct(Args&& ... args)
	{
		return ManagedObject(ConstructTemporary(std::forward<Args>...));
	}
}