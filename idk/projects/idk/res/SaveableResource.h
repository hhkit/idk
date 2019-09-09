#pragma once
#include <idk.h>
#include <iosfwd>

namespace idk
{
	namespace detail { template<typename T> struct tSavedHelper; }
	template<typename T>
	struct ResourceExtension
	{
		static constexpr string_view ext = "";
	};

#define RESOURCE_EXTENSION(Type, Extension)                                  \
	template<>                                                               \
	struct ResourceExtension<Type>                                           \
	{                                                                        \
		static_assert(Extension[0] == '.', "Extensions must begin with '.'");\
		static constexpr string_view ext = Extension;                        \
	};

	template<typename T, typename AutoSave = true_type>
	struct Saveable
		: ResourceExtension<T>
	{
		using ResourceExtension<T>::ext;
		string name = string{ reflect::get_type<T>().name() };

		static constexpr auto autosave = AutoSave::value;

		bool IsDirty() const { return _dirty; }
		void Dirty() { _dirty = true; };
	private:
		template<typename T>
		friend struct detail::tSavedHelper;
		friend class ResourceManager;

		bool _dirty = false;

		void Clean() { _dirty = false; };
	};
}