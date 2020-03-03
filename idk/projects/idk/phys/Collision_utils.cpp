#include "stdafx.h"
#include <phys/Collision_utils.h>
#include <util/hash_combine.h>

namespace idk
{
	size_t pair_hasher::operator()(const CollisionPair& collision_pair) const
	{
		auto hash = std::hash<size_t>{}(collision_pair.lhs.id);
		hash_combine(hash, collision_pair.rhs.id);
		return hash;
	}
}