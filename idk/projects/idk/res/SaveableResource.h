#pragma once
#include <idk.h>
#include <iosfwd>

namespace idk
{
	template<typename T>
	struct ResourceExtension
	{
		static_assert(false, "Please specialize ResourceExtension for type T");
	};

#define RESOURCE_EXTENSION(Type, Extension)                                 \
	template<>                                                              \
	struct ResourceExtension<T>                                             \
	{                                                                       \
		static_assert(Extension[0] == '.', "Extensions must begin with '.'")\
		static constexpr string_view ext = Extension;                       \
	};

	template<typename T>
	struct Saveable
		: ResourceExtension<T>
	{
		using ResourceExtension<T>::ext;
		string filename;

	private:
		friend class ResourceManager;
	};
}