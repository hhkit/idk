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
		template<typename T> RscHandle<T> Get(PathHandle mount_path);
		template<typename T> ResourceBundle::ResourceSpan<T> GetAll(PathHandle mount_path);
	};

	template<typename T>
	RscHandle<T> AssetImporter::Get(PathHandle mount_path)
	{
		auto span = Get(mount_path).GetAll<T>();
		if (span.size())
			return *span.begin();
		else
			return {};
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T> AssetImporter::GetAll(PathHandle mount_path)
	{
		return Get(mount_path).GetAll<T>();
	}
}