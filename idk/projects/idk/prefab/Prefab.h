#pragma once

#include <res/Resource.h>
#include <prefab/PrefabData.h>

namespace idk
{
	class Prefab : public Resource<Prefab>
	{
	public:
        // instantiate this prefab in the scene.
        // for simplicity's sake only prefab roots have PrefabInstance component attached.
        Handle<GameObject> Instantiate(Scene& scene) const;

	private:
        PrefabData _data;

        friend class PrefabFactory;
        friend class PrefabUtility;
	};
}