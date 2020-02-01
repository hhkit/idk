#include "stdafx.h"
#include "GhostMessage.h"

namespace idk
{
	GhostMessage& GhostMessage::AddPosition(vec3 trans)
	{
		state_mask |= GhostFlags::TRANSFORM_POS;
		position = trans;
		return *this;
	}
	GhostMessage& GhostMessage::AddRotation(quat rot)
	{
		state_mask |= GhostFlags::TRANSFORM_ROT;
		rotation.x = rot.x;
		rotation.y = rot.y;
		rotation.z = rot.z;
		rotation.w = rot.w;
		return *this;
	}
	GhostMessage& GhostMessage::AddScale(vec3 scl)
	{
		state_mask |= GhostFlags::TRANSFORM_SCALE;
		scale = scl;
		return *this;
	}
	opt<vec3> GhostMessage::GetPosition() const
	{
		if (state_mask & GhostFlags::TRANSFORM_POS)
			return position;
		return opt<vec3>();
	}
	opt<quat> GhostMessage::GetRotation() const
	{
		if (state_mask & GhostFlags::TRANSFORM_ROT)
		{
			quat rot;
			rot.x = rotation.x;
			rot.y = rotation.y;
			rot.z = rotation.z;
			rot.w = rotation.w;
			return rot;
		}
		return opt<quat>();
	}
	opt<vec3> GhostMessage::GetScale() const
	{
		if (state_mask & GhostFlags::TRANSFORM_SCALE)
			return scale;
		return opt<vec3>();
	}
}
