#include "stdafx.h"
#include "ManagedObj.h"

#include <script/ScriptSystem.h>
#include <script/MonoBehaviorEnvironment.h>
namespace idk::mono
{
	ManagedObject::ManagedObject(string_view _typename)
		: _typename{_typename}
	{
		auto& sv = Core::GetSystem<mono::ScriptSystem>().ScriptEnvironment();
		if (&sv)
		{
			if (auto type = sv.Type(_typename))
			{
				_gc_handle = mono_gchandle_new(type->ConstructTemporary(), false);
				return;
			}
		}
	}

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
		},
		_type {rhs._type}
	{
	}
	ManagedObject::ManagedObject(ManagedObject&& rhs)noexcept
		: _gc_handle{rhs._gc_handle}, _type {rhs._type}
	{
		rhs._gc_handle = 0;
		rhs._type = nullptr;
	}
	ManagedObject& ManagedObject::operator=(const ManagedObject& rhs) 
	{
		const auto fetch = rhs.Raw();
		const auto new_gc = fetch ? mono_gchandle_new(fetch, false) : 0;

		if (_gc_handle)
			mono_gchandle_free(_gc_handle);

		_gc_handle = new_gc;
		_type = rhs._type;
		return *this;
	}
	ManagedObject& ManagedObject::operator=(ManagedObject&& rhs)noexcept
	{
		std::swap(_gc_handle, rhs._gc_handle);
		std::swap(_type, rhs._type);
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
		return _gc_handle ? mono_gchandle_get_target(_gc_handle) : nullptr;
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
	const ManagedType* ManagedObject::Type() const noexcept
	{
		if (_gc_handle == 0)
			return nullptr;
		auto raw_class = Raw();
		auto klass = mono_object_get_class(raw_class);
		auto class_name = mono_class_get_name(klass);
		return Core::GetSystem<ScriptSystem>().ScriptEnvironment().Type(class_name);
	}
	string ManagedObject::TypeName() const
	{
		return *this ? mono_class_get_name(mono_object_get_class(Raw())) : _typename;
	}
	MonoClassField* ManagedObject::Field(string_view fieldname)
	{
		auto me = Raw();
		if (!me)
			return nullptr;

		auto klass = mono_object_get_class(me);
		return mono_class_get_field_from_name(klass, fieldname.data());
	}
}