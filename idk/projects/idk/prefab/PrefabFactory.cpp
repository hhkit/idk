#include "stdafx.h"
#include "PrefabFactory.h"
#include <serialize/serialize.h>
#include <sstream>

namespace idk
{

	unique_ptr<Prefab> PrefabFactory::Create()
	{
		return std::make_unique<Prefab>();
	}

	unique_ptr<Prefab> PrefabFactory::Create(FileHandle filepath)
	{
		//std::stringstream ss;
		//ss << filepath.Open(idk::FS_PERMISSIONS::READ).rdbuf();
		//string str = ss.str();

		//auto prefab = parse_text<Prefab>(str);
		return std::make_unique<Prefab>();
	}

}