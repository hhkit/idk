#pragma once
#include <script/ManagedType.h>
namespace idk::mono
{
	template<typename T>
	inline void ManagedObject::VisitImpl(T&& functor, int& depth)
	{
		for (void* iter = nullptr; auto field = mono_class_get_fields(mono_object_get_class(Raw()), &iter);)
		{
			auto field_name = string_view{ mono_field_get_name(field) };
			
			auto* klass = mono_class_from_mono_type(mono_field_get_type(field));
			auto* obj = mono_field_get_value_object(mono_domain_get(), field, Raw());
			++depth;
			if (klass == mono_get_int32_class()) // int
			{
				auto old_val = *s_cast<int*>(mono_object_unbox(obj));
				auto new_val = old_val;
				functor(field_name, new_val, depth);

				if (old_val != new_val)
					mono_field_set_value(Raw(), field, &new_val);
			}
			--depth;
		}
	}
}