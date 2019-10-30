#pragma once
#include <idk.h>

#include <mono/jit/jit.h>

#include <ds/result.h>
#include <script/ManagedThunk.h>

namespace idk::mono
{
	class ManagedObject;

	class ManagedType
	{
	public:
		explicit ManagedType(MonoClass* type);

		bool IsOrDerivedFrom(string_view type_name) const;

		MonoClass* Raw() const;

		template<typename ... Args>
		MonoObject* ConstructTemporary(Args&& ...) const;

		template<typename ... Args> 
		ManagedObject Construct(Args&&...) const;
		bool CacheThunk(string_view method_name, int param_count = 0);
		std::variant <ManagedThunk, MonoMethod*, std::nullopt_t> GetMethod(string_view method_name, int param_count = 0) const;
		monadic::result<ManagedThunk, std::nullopt_t> GetThunk(string_view method_name, int param_count = 0) const;
	private:
		MonoClass* type{};
		hash_table<string, ManagedThunk> thunks;

		MonoMethod* FindMethod(string_view method_name, int param_count) const;
	};

	template<typename ...Args>
	inline MonoObject* ManagedType::ConstructTemporary(Args&& ...) const
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
	inline ManagedObject ManagedType::Construct(Args&& ... args) const
	{
		auto retval = ManagedObject(ConstructTemporary(std::forward<Args>...));
		retval._type = this;
		return retval;
	}
}