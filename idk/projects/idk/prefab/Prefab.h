#pragma once

#include <res/Resource.h>

namespace idk
{
    struct PrefabData
    {
        vector<reflect::dynamic> components;
        vector<PrefabData> children;
    };

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