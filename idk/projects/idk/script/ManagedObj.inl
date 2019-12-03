#pragma once
#include <stack>

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
	functor(field_name.data(), new_val, -last_children);		\
														        \
	if (old_val != new_val)								        \
		mono_field_set_value(Raw(), field, &new_val);	        \
	last_children = 0;                                          \
	continue;                                                   \
}

#define MONO_COMPLEX_TYPE(REAL_TYPE, MONO_CLASS)                \
if (klass == MONO_CLASS)      \
{                                                               \
	auto old_val = s_cast<REAL_TYPE>(s_cast<float*>(mono_object_unbox(obj))); \
	auto new_val = old_val;								        \
	if (functor(field_name.data(), new_val, -last_children))	\
	{                                                           \
		last_children = 1;                                      \
		auto reflect = reflect::dynamic{ new_val };             \
		reflect.visit(functor);                  				\
	}                                                           \
	else                                                        \
		last_children = 0;                                      \
														        \
	if (old_val != new_val)								        \
		mono_field_set_value(Raw(), field, &new_val);	        \
	continue;                                                   \
}

#define MONO_BASE_TYPE_CONST(REAL_TYPE, MONO_CLASS)                   \
if (mono_class_get_name(klass) == string_view{MONO_CLASS})      \
{                                                                     \
	const auto old_val = s_cast<REAL_TYPE>(s_cast<float*>(mono_object_unbox(obj))); \
	functor(field_name.data(), old_val, depth);				          \
	continue;                                                         \
}

#define MONO_RESOURCE_TYPE(RES_TYPE)\
{																													\
	auto resource_klass = envi.Type(#RES_TYPE);																		\
	if (mono_class_get_name(klass) == string_view{#RES_TYPE}) 														\
	{																												\
		auto handle_field = mono_class_get_field_from_name(resource_klass->Raw(), "guid");							\
		auto old_val = [&]()->RscHandle<RES_TYPE>																	\
		{																											\
			if (obj == nullptr)																						\
				return RscHandle<RES_TYPE>{};																		\
			else																									\
			{																										\
				return RscHandle<RES_TYPE>{																			\
					*s_cast<Guid*>(mono_object_unbox(mono_field_get_value_object(mono_domain_get(), handle_field, obj)))\
				};																									\
			}																										\
		}();																										\
		auto new_val = old_val;																						\
																													\
		if (functor(field_name.data(), new_val, -last_children))													\
		{																											\
			last_children = 1;																						\
			auto reflect = reflect::dynamic{ new_val };																\
			reflect.visit(functor);																					\
		}																											\
		else																										\
			last_children = 0;																						\
																													\
		if (new_val != old_val)																						\
		{																											\
			auto insert_val = resource_klass->ConstructTemporary();													\
			mono_field_set_value(insert_val, handle_field, &new_val.guid);												\
			mono_field_set_value(Raw(), field, insert_val);															\
		}																											\
		continue;																									\
	}																												\
}
#define MONO_RESOURCE_TYPE_CONST(RES_TYPE)\
{																													\
	auto resource_klass = envi.Type(#RES_TYPE);																		\
	if (mono_class_get_name(klass) == string_view{#RES_TYPE}) 														\
	{																												\
		auto handle_field = mono_class_get_field_from_name(resource_klass->Raw(), "guid");							\
		auto old_val = [&]()->RscHandle<RES_TYPE>																	\
		{																											\
			if (obj == nullptr)																						\
				return RscHandle<RES_TYPE>{};																		\
			else																									\
			{																										\
				return RscHandle<RES_TYPE>{																			\
					*s_cast<Guid*>(mono_object_unbox(mono_field_get_value_object(mono_domain_get(), handle_field, obj)))\
				};																									\
			}																										\
		}();																										\
		auto new_val = old_val;																						\
																													\
		if (functor(field_name.data(), new_val, -last_children))													\
		{																											\
			last_children = 1;																						\
			auto reflect = reflect::dynamic{ new_val };																\
			reflect.visit(functor);																					\
		}																											\
		else																										\
			last_children = 0;																						\
		continue;																									\
	}																												\
}

namespace idk::mono
{
	template<typename T>
	inline void ManagedObject::Assign(string_view fieldname, T& obj)
	{
		using Actual = std::decay_t<T>;

		auto me = Raw();
		auto field = Field(fieldname);

		auto& envi = Core::GetSystem<ScriptSystem>().Environment();

		if (field)
		{
			if constexpr (!std::is_class_v<Actual>)
			{
				auto val = obj;
				(envi);//Unused in this path
				mono_field_set_value(me, field, &val);
			}
			if constexpr (std::is_same_v<Actual, reflect::dynamic>)
			{
				(envi);//Unused in this path
				reflect::dynamic& dyn = obj;
#define HACKY_ASSIGN(TYPE) if (dyn.is<TYPE>()) mono_field_set_value(me, field, &dyn.get<TYPE>());
				HACKY_ASSIGN(bool);
				HACKY_ASSIGN(short);
				HACKY_ASSIGN(unsigned short);
				HACKY_ASSIGN(int);
				HACKY_ASSIGN(unsigned int);
				HACKY_ASSIGN(int64_t);
				HACKY_ASSIGN(uint64_t);
				HACKY_ASSIGN(float);
				HACKY_ASSIGN(double);
				HACKY_ASSIGN(vec2);
				HACKY_ASSIGN(vec3);
				HACKY_ASSIGN(vec4);
				HACKY_ASSIGN(Guid);
#undef HACKY_ASSIGN
#define RSCHANDLE_ASSIGN(TYPE)                                                                                \
				if (dyn.is<RscHandle<TYPE>>())																  \
				{																							  \
					auto h = dyn.get<RscHandle<TYPE>>();													  \
					if (h)																					  \
					{																						  \
						auto resource_klass = envi.Type(#TYPE);												  \
						auto csharp_handle = resource_klass->ConstructTemporary();							  \
						auto handle_field = mono_class_get_field_from_name(resource_klass->Raw(), "guid");	  \
						mono_field_set_value(csharp_handle, handle_field, &h.guid);							  \
						mono_field_set_value(me, field, csharp_handle);										  \
					}																						  \
					else																					  \
						mono_field_set_value(me, field, nullptr);											  \
				}
				RSCHANDLE_ASSIGN(Prefab);
				RSCHANDLE_ASSIGN(MaterialInstance);
				RSCHANDLE_ASSIGN(Scene);
#undef RSCHANDLE_ASSIGN
			}
		}
	}

	template<typename T>
	void ManagedObject::Visit(T&& functor, bool ignore_privacy)
	{
		auto depth = int{};
		VisitImpl(std::forward<T>(functor), depth, ignore_privacy);
	}

	template<typename T>
	void ManagedObject::Visit(T&& functor, bool ignore_privacy) const
	{
		auto depth = int{};
		VisitImpl(std::forward<T>(functor), depth, ignore_privacy);
	}

	template<typename T>
	inline void ManagedObject::VisitImpl(T&& functor, int& depth, bool ignore_privacy)
	{
		++depth;
		auto last_children = 0;

		auto class_stack = [&]()
		{
			std::stack<MonoClass*> classes;
			//auto terminate_class = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Object")->Raw();
			auto curr_class = mono_object_get_class(Raw());
			while (mono_class_get_name(curr_class) != string_view{"Object"})
			{
				classes.push(curr_class);
				curr_class = mono_class_get_parent(curr_class);
			}

			return classes;
		}();

		while (!class_stack.empty())
		{
			auto curr_class = class_stack.top();
			class_stack.pop();
			for (void* iter = nullptr; auto field = mono_class_get_fields(curr_class, &iter);)
			{
				if (!ignore_privacy && Core::GetSystem<ScriptSystem>().Environment().IsPrivate(field))
					continue;

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
					auto old_val = [&]()
					{
						if (obj)
						{
							auto unboxed = unbox((MonoString*)obj);
							return string{ unboxed.get() };
						}
						else 
							return string{};
					}();
					auto new_val = old_val;

					functor(field_name.data(), new_val, depth);
					if (old_val != new_val)
						mono_field_set_value(Raw(), field, mono_string_new(mono_domain_get(), new_val.data()));

					continue;
				}
				
				auto& envi = Core::GetSystem<ScriptSystem>().Environment();

				MONO_COMPLEX_TYPE(vec2, envi.Type("Vector2")->Raw());
				MONO_COMPLEX_TYPE(vec3, envi.Type("Vector3")->Raw());
				MONO_COMPLEX_TYPE(vec4, envi.Type("Vector4")->Raw());

				MONO_RESOURCE_TYPE(MaterialInstance);
				MONO_RESOURCE_TYPE(Prefab);
				MONO_RESOURCE_TYPE(Scene);

				{
					using H_Type = GameObject;
					auto handle_klass = envi.Type("GameObject");
					if (klass == handle_klass->Raw())
					{
						auto handle_field = mono_class_get_field_from_name(handle_klass->Raw(), "handle");
						auto old_val = [&]() -> Handle<H_Type>
						{
							if (obj == nullptr)
								return Handle<H_Type>{};
							else
							{
								return Handle<H_Type>{
									*s_cast<uint64_t*>(mono_object_unbox(mono_field_get_value_object(mono_domain_get(), handle_field, obj)))
								};
							}
						}();

						auto new_val = old_val;

						if (functor(field_name.data(), new_val, -last_children))
						{
							last_children = 1;
							auto reflect = reflect::dynamic{ new_val };
							reflect.visit(functor);
						}
						else
							last_children = 0;

						if (new_val != old_val)
						{
							if (new_val.id)
							{
								auto insert_val = mono_object_new(mono_domain_get(), handle_klass->Raw());
								auto ctor = mono_class_get_method_from_name(handle_klass->Raw(), ".ctor", 1);
								void* args[] = { &new_val.id };
								mono_runtime_invoke(ctor, insert_val, args, nullptr);
								mono_field_set_value(Raw(), field, insert_val);
							}
							else
								mono_field_set_value(Raw(), field, nullptr);
						}

						continue;
					}
				}

				auto csharpcore = mono_get_corlib();
				MONO_BASE_TYPE(Guid, mono_class_from_name(csharpcore, "System", "Guid"));
			}
		}
		--depth;
	}

	template<typename T>
	inline void ManagedObject::VisitImpl(T&& functor, int& depth, bool ignore_privacy) const
	{
		++depth;
		auto last_children = 0;
		for (void* iter = nullptr; auto field = mono_class_get_fields(mono_object_get_class(Raw()), &iter);)
		{
			if (!ignore_privacy && Core::GetSystem<ScriptSystem>().Environment().IsPrivate(field))
				continue;

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
				auto old_val = [&]()
				{
					if (obj)
					{
						auto unboxed = unbox((MonoString*)obj);
						return string{ unboxed.get() };
					}
					else
						return string{};
				}();

				functor(field_name.data(), old_val, depth);

				continue;
			}

			auto& envi = Core::GetSystem<ScriptSystem>().Environment();

			MONO_BASE_TYPE_CONST(vec2, "Vector2");
			MONO_BASE_TYPE_CONST(vec3, "Vector3");
			MONO_BASE_TYPE_CONST(vec4, "Vector4");

			MONO_RESOURCE_TYPE_CONST(MaterialInstance);
			MONO_RESOURCE_TYPE_CONST(Prefab);
			MONO_RESOURCE_TYPE_CONST(Scene);

			auto csharpcore = mono_get_corlib();
			MONO_BASE_TYPE_CONST(Guid, mono_class_from_name(csharpcore, "System", "Guid"));
		}
		--depth;
	}
}

#undef MONO_BASE_TYPE