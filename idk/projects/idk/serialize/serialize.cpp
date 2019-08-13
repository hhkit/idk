#include "stdafx.h"
#include "serialize.h"
#include <reflect/reflect.h>
#include <res/Guid.h>

#include <json/json.hpp>
using json = nlohmann::json;

namespace idk
{
	template<>
	string serialize_text(const reflect::dynamic& obj)
	{
		if (obj.type.count() == 0)
		{
#define SERIALIZE_CASE(TYPE) case reflect::typehash<TYPE>() : return serialize_text(obj.get<TYPE>())
			switch (obj.type.hash())
			{
			SERIALIZE_CASE(int);
			SERIALIZE_CASE(bool);
			SERIALIZE_CASE(char);
			SERIALIZE_CASE(uint64_t);
			SERIALIZE_CASE(float);
			SERIALIZE_CASE(double);
			SERIALIZE_CASE(std::string);
			SERIALIZE_CASE(Guid);
			default: throw "Unhandled case?";
			}
#undef SERIALIZE_CASE
		}

		json j;
		vector<json*> stack{ &j };

		obj.visit([&](const char* name, auto&& arg, int depth_change)
		{
			using T = std::decay_t<decltype(arg)>;
			if (depth_change == -1)
				stack.pop_back();

			if constexpr (is_basic_serializable<T>::value)
				(*stack.back())[name] = serialize_text(arg);
			else
				stack.push_back(&((*stack.back())[name] = json::object()));
		});

		return j.dump(2);
	}
}