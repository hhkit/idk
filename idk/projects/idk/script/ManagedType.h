#pragma once
#include <idk.h>

#include <mono/jit/jit.h>

#include <ds/result.h>
#include <script/ManagedThunk.h>

namespace idk::mono
{
	class ManagedObject;
	struct RPCMethod
	{
		MonoMethod* method{};
		bool has_message_info_arg = false;

		explicit operator bool() const { return method != nullptr; }
	};

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

		void CacheMessages();
		bool CacheThunk(string_view method_name, int param_count = 0);
		void FindRPCs();
		RPCMethod GetRPC(string_view method) const;

		std::variant <ManagedThunk, MonoMethod*, std::nullopt_t> GetMethod(string_view method_name, int param_count = 0) const;
		opt<ManagedThunk> GetThunk(string_view method_name, int param_count = 0) const;
	private:
		MonoClass* type{};
		string name;
		hash_table<string, ManagedThunk> thunks;
		hash_table<string, RPCMethod> rpcs;

		MonoMethod* FindMethod(string_view method_name, int param_count) const;
	};

	template<typename ...Args>
	inline MonoObject* ManagedType::ConstructTemporary(Args&& ... args) const
	{
		const auto domain = mono_domain_get();
		auto obj = mono_object_new(domain, type);

		// construct 
		if (!mono_class_is_valuetype(type))
		{
			if constexpr (sizeof...(Args) == 0)
				mono_runtime_object_init(obj);
			else
			{
				void* argv[sizeof...(args)] = { &args... };
				auto method = mono_class_get_method_from_name(type, ".ctor", sizeof...(args));
				mono_runtime_invoke(method, obj, argv, nullptr);
			}
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