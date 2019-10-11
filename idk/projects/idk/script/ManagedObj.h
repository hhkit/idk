#pragma once
#include <mono/jit/jit.h>

namespace idk::mono
{
	class ManagedType;

	class ManagedObject
	{
	public:
		ManagedObject(MonoObject* obj);
		ManagedObject(const ManagedObject&);
		ManagedObject(ManagedObject&&) noexcept;
		ManagedObject& operator=(const ManagedObject&);
		ManagedObject& operator=(ManagedObject&&) noexcept;
		~ManagedObject();

		MonoObject* Fetch() const noexcept;
	private:
		uint32_t _gc_handle{};
		ManagedType* _type{};
	};

}