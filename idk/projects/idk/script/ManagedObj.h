#pragma once
#include <mono/jit/jit.h>

namespace idk::mono
{
	class ManagedType;

	class ManagedObject
	{
	public:
		ManagedObject() noexcept = default;
		ManagedObject(MonoObject* obj);
		ManagedObject(const ManagedObject&);
		ManagedObject(ManagedObject&&) noexcept;
		ManagedObject& operator=(const ManagedObject&);
		ManagedObject& operator=(ManagedObject&&) noexcept;
		~ManagedObject();

		// properties
		MonoObject*        Raw() const noexcept;
		const ManagedType* Type() noexcept;

		// mutators
		void Assign(string_view field, MonoObject* obj);
		template<typename T>
		void Assign(string_view field, T& obj);
		template<typename T>
		T Get(string_view field);

		explicit operator bool() const;


		template<typename T>
		void VisitImpl(T&& functor, int& depth_change);
	private:
		uint32_t _gc_handle{};
		const ManagedType* _type{};

		MonoClassField* Field(string_view field);

		friend class ManagedType;
	};

	template<typename T>
	inline void ManagedObject::Assign(string_view fieldname, T& obj)
	{
		auto me = Raw();
		auto field = Field(fieldname);

		if (field)
		{
			if constexpr (!std::is_class_v<std::decay_t<T>>)
			{
				auto val = obj;
				mono_field_set_value(me, field, &val);
			}
		}
	}
}

#include "ManagedObj.inl"