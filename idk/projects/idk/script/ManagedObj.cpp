#include "stdafx.h"
#include "ManagedObj.h"

namespace idk::mono
{
	ManagedObject::ManagedObject(MonoObject* obj)
		: _gc_handle{mono_gchandle_new(obj, false)}
	{
	}
	ManagedObject::ManagedObject(const ManagedObject& rhs)
		: _gc_handle
		{ 
			[](const ManagedObject& rhs) -> uint32_t
			{
				const auto fetch = rhs.Fetch();
				return fetch ? mono_gchandle_new(fetch, false) : 0;
			}(rhs)
		}
	{
	}
	ManagedObject::ManagedObject(ManagedObject&& rhs)noexcept
		: _gc_handle{rhs._gc_handle}
	{
		rhs._gc_handle = 0;
	}
	ManagedObject& ManagedObject::operator=(const ManagedObject& rhs) 
	{
		const auto fetch = rhs.Fetch();
		const auto new_gc = fetch ? mono_gchandle_new(fetch, false) : 0;

		if (_gc_handle)
			mono_gchandle_free(_gc_handle);

		_gc_handle = new_gc;
		return *this;
	}
	ManagedObject& ManagedObject::operator=(ManagedObject&& rhs)noexcept
	{
		std::swap(_gc_handle, rhs._gc_handle);
		return *this;
	}
	ManagedObject::~ManagedObject()
	{
		if (_gc_handle)
			mono_gchandle_free(_gc_handle);
	}
	MonoObject* ManagedObject::Fetch() const noexcept
	{
		return mono_gchandle_get_target(_gc_handle);
	}
}