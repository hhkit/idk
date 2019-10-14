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

		void Assign(string_view field, MonoObject* obj);
		template<typename T>
		void Assign(string_view field, T& obj);
		template<typename T>
		T Get(string_view field);
		MonoObject* Fetch() const noexcept;
	private:
		uint32_t _gc_handle{};
		ManagedType* _type{};

		MonoClassField* Field(string_view field);
	};

	template<typename T>
	inline void ManagedObject::Assign(string_view fieldname, T& obj)
	{
		auto me = Fetch();
		auto field = Field(fieldname);

		if (field)
		{
			if constexpr(std::common_type<std::decay_t<T>>)
				mono_field_set_value(me, field, &obj);
		}
	}

}