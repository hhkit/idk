#pragma once
#include <res/ResourceFactory.h>
#include <res/SaveableResource.h>
#include <serialize/serialize.h>
namespace idk
{
	template<typename T>
	class EasyFactory
		: public ResourceFactory<T>
	{
		virtual unique_ptr<T> GenerateDefaultResource();
		virtual unique_ptr<T> Create();
		virtual unique_ptr<T> Create(PathHandle filepath);
	};
}

namespace idk
{
	template<typename T>
	unique_ptr<T> EasyFactory<T>::GenerateDefaultResource()
	{
		return std::make_unique<T>();
	}
	template<typename T>
	inline unique_ptr<T> EasyFactory<T>::Create()
	{
		return std::make_unique<T>();
	}
	template<typename T>
	inline unique_ptr<T> EasyFactory<T>::Create(PathHandle filepath)
	{
		auto ptr = std::make_unique<T>();

		if constexpr (has_tag_v<T, Saveable>)
			parse_text(stringify(filepath.Open(FS_PERMISSIONS::READ)), *ptr);
		return ptr;
	}
}