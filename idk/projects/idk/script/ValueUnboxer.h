#pragma once
#include <idk.h>
#include <mono/jit/jit.h>

namespace idk::mono
{
	unique_ptr<char, decltype(&mono_free)> unbox(MonoString* obj);
}