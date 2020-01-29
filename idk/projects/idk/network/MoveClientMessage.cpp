#include "stdafx.h"
#include "MoveClientMessage.h"

namespace idk
{
	MoveClientMessage& MoveClientMessage::AddTranslation(vec3 trans)
	{
		state_mask |= GhostFlags::TRANSFORM_POS;
		translation = trans;
		return *this;
	}
	MoveClientMessage& MoveClientMessage::AddRotation(quat rot)
	{
		state_mask |= GhostFlags::TRANSFORM_ROT;
		rotation.x = rot.x;
		rotation.y = rot.y;
		rotation.z = rot.z;
		rotation.w = rot.w;
		return *this;
	}
	MoveClientMessage& MoveClientMessage::AddScale(vec3 scl)
	{
		state_mask |= GhostFlags::TRANSFORM_SCALE;
		scale = scl;
		return *this;
	}
	opt<vec3> MoveClientMessage::GetTranslation() const
	{
		if (state_mask & GhostFlags::TRANSFORM_POS)
			return translation;
		return opt<vec3>();
	}
	opt<quat> MoveClientMessage::GetRotation() const
	{
		quat rot;
		if (state_mask & GhostFlags::TRANSFORM_ROT)
		{
			rot.x = rotation.x;
			rot.y = rotation.y;
			rot.z = rotation.z;
			rot.w = rotation.w;
			return rot;
		}
		return opt<quat>();
	}
	opt<vec3> MoveClientMessage::GetScale() const
	{
		if (state_mask & GhostFlags::TRANSFORM_SCALE)
			return scale;
		return opt<vec3>();
	}
}