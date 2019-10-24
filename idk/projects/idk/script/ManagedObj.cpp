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
	string_view ManagedObject::TypeName() const
	{
		return mono_class_get_name(mono_object_get_class(Raw()));
	}

#define TO_VAL(NATIVE_CLASS, MONO_CLASS) \
	if (klass == MONO_CLASS) new_val.data = *s_cast<NATIVE_CLASS*>(mono_object_unbox(obj));

	MonoValue ManagedObject::Value() const
	{
		MonoValue v;
		v.name = TypeName();

		auto& envi = Core::GetSystem<ScriptSystem>().Environment();
		auto method_seeker = envi.Type("IDK")->GetMethod("Main", 1);
		auto cstruct = vector<MonoValue>{};
		for (void* iter = nullptr; auto field = mono_class_get_fields(mono_object_get_class(Raw()), &iter);)
		{
			if (method_seeker.index() != 1 || !mono_method_can_access_field(std::get<MonoMethod*>(method_seeker), field))
				continue;

			MonoValue new_val;
			new_val.name = mono_field_get_name(field);

			auto* klass = mono_class_from_mono_type(mono_field_get_type(field));
			auto* obj = mono_field_get_value_object(mono_domain_get(), field, Raw());
			TO_VAL(bool, mono_get_boolean_class());
			TO_VAL(char, mono_get_char_class());
			TO_VAL(short, mono_get_int16_class());
			TO_VAL(int, mono_get_int32_class());
			TO_VAL(long, mono_get_int64_class());
			TO_VAL(unsigned short, mono_get_uint16_class());
			TO_VAL(unsigned int, mono_get_uint32_class());
			TO_VAL(unsigned long, mono_get_uint64_class());
			TO_VAL(float, mono_get_single_class());
			TO_VAL(double, mono_get_double_class());

			if (klass == mono_get_string_class())
			{
				auto unboxed = unbox((MonoString*)obj);
				new_val.data = string{ unboxed.get() };
			}

			TO_VAL(Guid, mono_class_from_name(mono_get_corlib(), "System", "Guid"));
			cstruct.emplace_back(new_val);
		}

		v.data = std::move(cstruct);

		return v;
	}

#define FROM_VAL(NATIVE_CLASS, MONO_CLASS) \
	if (klass == MONO_CLASS && elem.data.index() == index_in_variant_v<NATIVE_CLASS, MonoVariant>)\
		mono_field_set_value(Raw(), field, c_cast<NATIVE_CLASS*>(&std::get<NATIVE_CLASS>(elem.data)));

	void ManagedObject::Value(span<const MonoValue> values)
	{
		auto* raw_type = mono_object_get_class(Raw());
		for (auto& elem : values)
		{
			auto field = mono_class_get_field_from_name(raw_type, elem.name.data());
			auto klass = mono_type_get_class(mono_field_get_type(field));
			FROM_VAL(bool, mono_get_boolean_class());
			FROM_VAL(char, mono_get_char_class());
			FROM_VAL(short, mono_get_int16_class());
			FROM_VAL(int, mono_get_int32_class());
			FROM_VAL(long, mono_get_int64_class());
			FROM_VAL(unsigned short, mono_get_uint16_class());
			FROM_VAL(unsigned int, mono_get_uint32_class());
			FROM_VAL(unsigned long, mono_get_uint64_class());
			FROM_VAL(float, mono_get_single_class());
			FROM_VAL(double, mono_get_double_class());
		}
	}

	MonoClassField* ManagedObject::Field(string_view fieldname)
	{
		auto me = Raw();
		auto klass = mono_object_get_class(me);
		return mono_class_get_field_from_name(klass, fieldname.data());
	}
}