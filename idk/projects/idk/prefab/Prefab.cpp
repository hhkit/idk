#include "stdafx.h"
#include "Prefab.h"

#include <prefab/PrefabUtility.h>
#include <core/GameObject.h>

namespace idk
{
    Handle<GameObject> Prefab::Instantiate(Scene& scene) const
    {
        return PrefabUtility::Instantiate(GetHandle(), scene);
    }
}