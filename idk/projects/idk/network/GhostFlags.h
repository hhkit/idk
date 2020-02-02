#pragma once

namespace idk
{
	struct GhostFlags
	{
		enum Flags
		{
			TRANSFORM_POS      = 1 << 0,
			TRANSFORM_ROT      = 1 << 1,
			TRANSFORM_SCALE    = 1 << 2,
			RIGIDBODY_PREV_POS = 1 << 3,
		};
	};
}