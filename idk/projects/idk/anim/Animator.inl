#pragma once

namespace idk
{
	template<typename T>
	hash_table<string, T>& Animator::GetParamTable()
	{
		if constexpr (std::is_same_v<T, anim::IntParam>)
		{
			return int_vars;
		}
		else if constexpr (std::is_same_v<T, anim::FloatParam>)
		{
			return float_vars;
		}
		else if constexpr (std::is_same_v<T, anim::BoolParam>)
		{
			return bool_vars;
		}
		else if constexpr (std::is_same_v<T, anim::TriggerParam>)
		{
			return trigger_vars;
		}
		else
		{
			throw("???");
		}
	}

	template<typename T>
	const hash_table<string, T>& Animator::GetParamTable() const
	{
		if constexpr (std::is_same_v<T, anim::IntParam>)
		{
			return int_vars;
		}
		else if constexpr (std::is_same_v<T, anim::FloatParam>)
		{
			return float_vars;
		}
		else if constexpr (std::is_same_v<T, anim::BoolParam>)
		{
			return bool_vars;
		}
		else if constexpr (std::is_same_v<T, anim::TriggerParam>)
		{
			return trigger_vars;
		}
		else
		{
			throw("???");
		}
	}

	template<typename T>
	const T& Animator::GetParam(string_view name) const
	{
		const auto& param_table = GetParamTable<T>();
		auto res = param_table.find(name.data());
		if (res != param_table.end())
			return res->second;

		return anim::null_param<T>();
	}

	template<typename T>
	T& Animator::GetParam(string_view name)
	{
		hash_table<string, T>& param_table = GetParamTable<T>();
		auto res = param_table.find(name.data());
		if (res != param_table.end())
			return res->second;

		return anim::null_param<T>();
	}

	template<typename ParamType, typename ValueType>
	bool Animator::SetParam(string_view name, ValueType val, bool def_val)
	{
		auto& param_table = GetParamTable<ParamType>();
		auto res = param_table.find(name.data());
		if (res != param_table.end())
		{
			if (def_val)
				res->second.def_val = val;

			res->second.val = val;
			return true;
		}

		return false;
	}

	template<typename T>
	void Animator::AddParam(string_view name)
	{
		auto& param_table = GetParamTable<T>();
		auto res = param_table.find(name.data());
		T param{name.data(), true};

		int count = -1;
		while (res != param_table.end())
		{
			res = param_table.find(param.name + " " + std::to_string(++count));
		}

		if (count >= 0)
			param.name += " " + std::to_string(count);

		param_table.emplace(param.name, param);
	}

	template<typename T>
	bool Animator::RemoveParam(string_view name)
	{
		auto& param_table = GetParamTable<T>();
		auto res = param_table.find(name.data());
		if (res != param_table.end())
		{
			param_table.erase(res);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Animator::RenameParam(string_view from, string_view to)
	{
		string from_str{ from }, to_str{to};

		auto& param_table = GetParamTable<T>();
		auto from_res = param_table.find(from_str);
		auto to_res = param_table.find(to_str);

		// from must be found, to must not be found
		if (from_res != param_table.end() && to_res == param_table.end())
		{
			// Rename all params that every condition is using
			for (auto& layer : layers)
			{
				for (auto& state : layer.anim_states)
				{
					for (auto& transition : state.transitions)
					{
						for (auto& condition : transition.conditions)
						{
							if (condition.param_name == from_str)
								condition.param_name = to_str;
						}
					}
				}
			}

			T copy = from_res->second;
			copy.name = to_str;

			param_table.erase(from_res);
			param_table.emplace(to_str, copy);
			return true;
		}

		return false;
	}
}