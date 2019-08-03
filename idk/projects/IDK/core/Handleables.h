#pragma once
#include <idk.h>

namespace idk
{
	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Handleables = tuple<
		class GameObject,
		class Transform,
		class Parent
	>;
}