#include "stdafx.h"
#include "ValueUnboxer.h"

namespace idk::mono
{
	unique_ptr<char, decltype(&mono_free)> unbox(MonoString* obj)
	{
		return unique_ptr<char, decltype(&mono_free)>(mono_string_to_utf8(obj), mono_free);
	}
}
