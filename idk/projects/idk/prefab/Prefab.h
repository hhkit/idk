#pragma once

#include <res/Resource.h>
#include <prefab/PrefabData.h>

namespace idk
{
	class Prefab : public Resource<Prefab>
	{
    public:
        static void Save(Handle<GameObject> go, FileHandle save_path);

	public:
        Handle<GameObject> Instantiate(Scene& scene) const;

	private:
        PrefabData _data;

        friend class PrefabFactory;
	};
}