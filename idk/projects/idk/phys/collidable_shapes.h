#pragma once
#include <math/shapes/aabb.h>
#include <math/shapes/box.h>
#include <math/shapes/sphere.h>
#include <math/shapes/capsule.h>

namespace idk
{
	using CollidableShapes = variant<box, sphere, capsule>;
}