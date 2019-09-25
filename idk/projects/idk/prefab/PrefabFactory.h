#pragma once

#include <idk.h>
#include <prefab/Prefab.h>
#include <res/EasyFactory.h>
#include <res/ResourceFactory.h>

namespace idk
{
	using PrefabFactory = EasyFactory<Prefab>;

	class PrefabLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle filepath) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& bundle) override;
	};
}