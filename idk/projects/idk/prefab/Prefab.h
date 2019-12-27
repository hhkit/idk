#pragma once
#include <core/Handle.h>
#include <res/Resource.h>
#include <prefab/PrefabData.h>
#include <res/SaveableResource.h>

namespace idk
{
    class GameObject;
	class Prefab : public Resource<Prefab>, public Saveable<Prefab>
	{
	public:
        // instantiate this prefab in the scene.
        // for simplicity's sake only prefab roots have PrefabInstance component attached.
        Handle<GameObject> Instantiate(Scene& scene) const;

        // data[0] = self
        // the rest refer to children
        vector<PrefabData> data;

		EXTENSION(".idp");
	};
}