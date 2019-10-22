#pragma once
#include <script/ManagedType.h>
#include <script/ValueUnboxer.h>
#include <script/ScriptSystem.h>
#include <script/MonoEnvironment.h>
#include "ManagedObj.h"

#define MONO_BASE_TYPE(REAL_TYPE, MONO_CLASS)                   \
if (klass == MONO_CLASS)                                        \
{                                                               \
	auto old_val = *s_cast<REAL_TYPE*>(mono_object_unbox(obj)); \
	auto new_val = old_val;								        \
	functor(field_name.data(), new_val, depth);				        \
														        \
	if (old_val != new_val)								        \
		mono_field_set_value(Raw(), field, &new_val);	        \
	continue;                                                   \
}

#define MONO_BASE_TYPE_CONST(REAL_TYPE, MONO_CLASS)                   \
if (klass == MONO_CLASS)                                              \
{                                                                     \
	auto old_val = *s_cast<const REAL_TYPE*>(mono_object_unbox(obj)); \
	functor(field_name.data(), old_val, depth);				              \
	continue;                                                         \
}

namespace idk::mono
{
	template<typename T>
	void ManagedObject::Visit(T&& functor)
	{
		auto depth = int{};
		VisitImpl(std::forward<T>(functor), depth);
	}

	template<typename T>
	void ManagedObject::Visit(T&& functor) const
	{
		auto depth = int{};
		VisitImpl(std::forward<T>(functor), depth);
	}

	template<typename T>
	inline void ManagedObject::VisitImpl(T&& functor, int& depth)
	{
		++depth;
		for (void* iter = nullptr; auto field = mono_class_get_fields(mono_object_get_class(Raw()), &iter);)
		{
			auto field_name = string_view{ mono_field_get_name(field) };
			
			auto* klass = mono_class_from_mono_type(mono_field_get_type(field));
			auto* obj = mono_field_get_value_object(mono_domain_get(), field, Raw());
			MONO_BASE_TYPE(bool, mono_get_boolean_class());
			MONO_BASE_TYPE(char, mono_get_char_class());
			MONO_BASE_TYPE(short, mono_get_int16_class());
			MONO_BASE_TYPE(int, mono_get_int32_class());
			MONO_BASE_TYPE(long, mono_get_int64_class());
			MONO_BASE_TYPE(unsigned short, mono_get_uint16_class());
			MONO_BASE_TYPE(unsigned int, mono_get_uint32_class());
			MONO_BASE_TYPE(unsigned long, mono_get_uint64_class());
			MONO_BASE_TYPE(float, mono_get_single_class());
			MONO_BASE_TYPE(double, mono_get_double_class());

			if (klass == mono_get_string_class())
			{
				auto unboxed = unbox((MonoString*) obj);
				auto old_val = string{ unboxed.get() };
				auto new_val = old_val;

				functor(field_name.data(), new_val, depth);
				if (old_val != new_val)
					mono_field_set_value(Raw(), field, mono_string_new(mono_domain_get(), new_val.data()));

				continue;
			}

			auto& envi = Core::GetSystem<ScriptSystem>().Environment();
			
			MONO_BASE_TYPE(vec2, envi.Type("Vector2")->Raw());
			MONO_BASE_TYPE(vec3, envi.Type("Vector3")->Raw());
			MONO_BASE_TYPE(vec4, envi.Type("Vector4")->Raw());
			
			auto csharpcore = mono_get_corlib();
			MONO_BASE_TYPE(Guid, mono_class_from_name(csharpcore, "System", "Guid"));
		}
		--depth;
	}

	template<typename T>
	inline void ManagedObject::VisitImpl(T&& functor, int& depth) const
	{
		++depth;
		for (void* iter = nullptr; auto field = mono_class_get_fields(mono_object_get_class(Raw()), &iter);)
		{
			auto field_name = string_view{ mono_field_get_name(field) };

			auto* klass = mono_class_from_mono_type(mono_field_get_type(field));
			auto* obj = mono_field_get_value_object(mono_domain_get(), field, Raw());
			MONO_BASE_TYPE_CONST(bool, mono_get_boolean_class());
			MONO_BASE_TYPE_CONST(char, mono_get_char_class());
			MONO_BASE_TYPE_CONST(short, mono_get_int16_class());
			MONO_BASE_TYPE_CONST(int, mono_get_int32_class());
			MONO_BASE_TYPE_CONST(long, mono_get_int64_class());
			MONO_BASE_TYPE_CONST(unsigned short, mono_get_uint16_class());
			MONO_BASE_TYPE_CONST(unsigned int, mono_get_uint32_class());
			MONO_BASE_TYPE_CONST(unsigned long, mono_get_uint64_class());
			MONO_BASE_TYPE_CONST(float, mono_get_single_class());
			MONO_BASE_TYPE_CONST(double, mono_get_double_class());

			if (klass == mono_get_string_class())
			{
				auto unboxed = unbox((MonoString*)obj);
				auto old_val = string{ unboxed.get() };

				functor(field_name.data(), old_val, depth);

				continue;
			}

			auto& envi = Core::GetSystem<ScriptSystem>().Environment();

			MONO_BASE_TYPE_CONST(vec2, envi.Type("Vector2")->Raw());
			MONO_BASE_TYPE_CONST(vec3, envi.Type("Vector3")->Raw());
			MONO_BASE_TYPE_CONST(vec4, envi.Type("Vector4")->Raw());

			auto csharpcore = mono_get_corlib();
			MONO_BASE_TYPE_CONST(Guid, mono_class_from_name(csharpcore, "System", "Guid"));
		}
		--depth;
	}
}

#undef MONO_BASE_TYPE