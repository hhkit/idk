#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include "GhostFlags.h"

namespace idk
{
	class MoveClientMessage
		: public yojimbo::Message
	{
	public:
		NetworkID network_id;
		unsigned state_mask{};

		vec3 translation;
		quat rotation;
		vec3 scale;

		MoveClientMessage& AddTranslation(vec3 trans);
		MoveClientMessage& AddRotation(quat rot);
		MoveClientMessage& AddScale(vec3 scl);

		opt<vec3> GetTranslation() const;
		opt<quat> GetRotation() const;
		opt<vec3> GetScale() const;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, network_id, 0, 4096);
			serialize_int(stream, state_mask, 0, 0x7FFFFFFF);
			if (state_mask & GhostFlags::TRANSFORM_POS)
			{
				serialize_float(stream, translation.x);
				serialize_float(stream, translation.y);
				serialize_float(stream, translation.z);
			}

			if (state_mask & GhostFlags::TRANSFORM_ROT)
			{
				serialize_float(stream, rotation.x);
				serialize_float(stream, rotation.y);
				serialize_float(stream, rotation.z);
				serialize_float(stream, rotation.w);
			}

			if (state_mask & GhostFlags::TRANSFORM_SCALE)
			{
				serialize_float(stream, scale.x);
				serialize_float(stream, scale.y);
				serialize_float(stream, scale.z);
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}