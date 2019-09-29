#include "stdafx.h"
#include "PrefabFactory.h"
#include <serialize/serialize.h>
#include <sstream>

namespace idk
{

	ResourceBundle PrefabLoader::LoadFile(PathHandle filepath)
	{
		std::stringstream ss;
		ss << filepath.Open(idk::FS_PERMISSIONS::READ).rdbuf();
		string str = ss.str();

		auto prefab = Core::GetResourceManager().LoaderEmplaceResource<Prefab>();
		parse_text(str, prefab->data);
		return prefab;
	}

	ResourceBundle PrefabLoader::LoadFile(PathHandle filepath, const MetaBundle& bundle)
	{
		filepath;
		bundle;
		return ResourceBundle();
	}

}