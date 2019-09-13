#pragma once
#include <res/Resource.h>
#include <util/enum.h>

namespace idk
{
	ENUM(TextureTarget, int,
		PosX, NegX,
		PosY, NegY,
		PosZ, NegZ
		);

	class CubeMap
		: public Resource<CubeMap>
	{
	public:
		virtual void* GetID(TextureTarget) = 0;
		virtual ~CubeMap() = default;
	};
}