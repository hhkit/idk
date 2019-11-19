#pragma once
#include <core/ISystem.h>
#include <res/ResourceBundle.h>
namespace idk
{
	class AssetImporter
		: public ISystem
	{
	public:
		virtual void CheckImportDirectory() = 0;
		virtual ResourceBundle Get(PathHandle path) = 0;
		template<typename T> ResourceBundle::ResourceSpan<T> GetAll(string_view mount_path);
	};

	template<typename T>
	ResourceBundle::ResourceSpan<T> AssetImporter::GetAll(string_view mount_path)
	{
		auto itr = bundles.find(mount_path);
		if (itr != bundles.end())
			return itr->second.GetAll<T>();

		return {};
	}
}