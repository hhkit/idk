#pragma once
#include <mono/jit/jit.h>

namespace idk::mono
{
	class ManagedObject
	{
	public:
		ManagedObject(MonoObject* obj = nullptr);
		ManagedObject(ManagedObj&& rhs);
		ManagedObject& operator=(ManagedObj&& rhs);
		~ManagedObject();
	private:
		MonoObject* object{};
	};
}