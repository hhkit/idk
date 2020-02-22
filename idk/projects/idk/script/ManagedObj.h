#pragma once
#include <mono/jit/jit.h>
#include <idk.h>
#include <meta/variant.inl>

namespace idk::mono
{
	using CSharpObjectVariant = variant<bool, unsigned char, char, unsigned short, short, unsigned, int, unsigned long long, long long, float, double, string, Handle<GameObject>> ;

	class ManagedType;

	class ManagedObject
	{
	public:
		ManagedObject() = default;
		explicit ManagedObject(string_view _typename);
		ManagedObject(MonoObject* obj);
		ManagedObject(const ManagedObject&);
		ManagedObject(ManagedObject&&) noexcept;
		ManagedObject& operator=(const ManagedObject&);
		ManagedObject& operator=(ManagedObject&&) noexcept;
		~ManagedObject();

		// properties
		MonoObject*        Raw() const noexcept;
		const ManagedType* Type() noexcept;
		const ManagedType* Type() const noexcept;
		string             TypeName() const;

		// mutators
		void Assign(string_view field, MonoObject* obj);
		template<typename T>
		void Assign(string_view field, T&& obj);
		template<typename T>
		T Get(string_view field);

		explicit operator bool() const;

		template<typename T> void Visit(T&& functor, bool ignore_privacy = false);
		template<typename T> void Visit(T&& functor, bool ignore_privacy = false) const;

		template<typename T> void VisitImpl(T&& functor, int& depth_change, bool ignore_privacy);
		template<typename T> void VisitImpl(T&& functor, int& depth_change, bool ignore_privacy) const;
	private:
		uint32_t _gc_handle{};
		const ManagedType* _type{};
		string _typename;
		hash_table<string, CSharpObjectVariant> _variables;

		MonoClassField* Field(string_view field);

		friend class ManagedType;
	};
}

