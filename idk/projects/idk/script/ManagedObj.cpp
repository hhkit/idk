#include "stdafx.h"
#include "ManagedObj.h"

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
namespace idk::mono
{
	ManagedObject::ManagedObject(MonoObject* obj)
		: _gc_handle{mono_gchandle_new(obj, false)}
	{
	}
	ManagedObject::ManagedObject(const ManagedObject& rhs)
		: _gc_handle
		{ 
			rhs._gc_handle ?
			[](const ManagedObject& rhs) -> uint32_t
			{
				const auto fetch = rhs.Raw();
				return fetch ? mono_gchandle_new(fetch, false) : 0;
			}(rhs)
			: 0
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
		const auto fetch = rhs.Raw();
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

	void ManagedObject::Assign(string_view fieldname, MonoObject* obj)
	{
		auto me = Raw();
		auto field = Field(fieldname);
		if (field)
			mono_field_set_value(me, field, obj);
	}

	MonoObject* ManagedObject::Raw() const noexcept
	{
		return mono_gchandle_get_target(_gc_handle);
	}
	ManagedObject::operator bool() const
	{
		return _gc_handle;
	}
	const ManagedType* ManagedObject::Type() noexcept
	{
		if (_type == nullptr)
		{
			if (_gc_handle == 0)
				return nullptr;

			_type = Core::GetSystem<ScriptSystem>().ScriptEnvironment().Type(mono_class_get_name(mono_object_get_class(Raw())));
		}
		return _type;
	}
	MonoClassField* ManagedObject::Field(string_view fieldname)
	{
		auto me = Raw();
		auto klass = mono_object_get_class(me);
		return mono_class_get_field_from_name(klass, fieldname.data());
	}
}