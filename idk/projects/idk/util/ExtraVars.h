#pragma once
#include <idk.h>
#include <meta/meta.inl>
namespace idk
{
	struct ExtraVars
	{
		using variant_t = std::variant<bool, float, string, int,void*>;
		std::unordered_map < string, variant_t> extra_vars;
		bool SetIfUnset(string name, variant_t v)
		{
			return extra_vars.emplace(std::move(name), std::move(v)).second;
		}

		bool Unset(const string& name)
		{
			auto itr = extra_vars.find(name);
			bool result = extra_vars.end()!=itr;
			if (result)
				extra_vars.erase(itr);
			return result;
		}
		void Set(const string& name, variant_t v)
		{
			extra_vars[std::move(name)] = std::move(v);
		}
		std::optional<variant_t> Get(const string& name)const
		{
			auto res = std::optional<variant_t>{};
			auto itr = extra_vars.find(name);
			if (itr != extra_vars.end())
				res = itr->second;
			return res;
		}
		variant_t& Get(const string& name)
		{
			return extra_vars[name];
		}
		template<typename T, typename res_t = std::optional<T> ,typename = std::enable_if_t<is_variant_member_v<T,variant_t>>>
		res_t Get(const string& name) const
		{
			res_t res{};
			auto v = Get(name);
			if (v && v->index()==index_in_variant_v<T,variant_t>)
				res = std::get<T>(*v);
			return res;
		}
		auto begin()const
		{
			return extra_vars.begin();
		}
		auto end()const
		{
			return extra_vars.end();
		}
		auto begin()
		{
			return extra_vars.begin();
		}
		auto end()
		{
			return extra_vars.end();
		}
		auto size()const
		{
			return extra_vars.size();
		}
	};
}