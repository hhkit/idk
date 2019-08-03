#pragma once
#include <idk.h>
#include "ObjectHandle.h"

namespace idk
{
	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Handleables = tuple<
		class GameObject
	>;

	template <typename T>
	class ObjectPool;

	template<typename T>
	class Handleable
	{
	public:
		const ObjectHandle<T>& GetHandle() { return handle; }
	private:
		ObjectHandle<T> handle;
		friend class ObjectPool<T>;
	};
}