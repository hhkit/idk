#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <network/GhostFlags.h>
namespace idk
{
	class GhostMessage
		: public yojimbo::Message
	{
	public:
		unsigned network_id;

		StateMask state_mask{};
		vec3 position;
		quat rotation;
		vec3 scale;

		GhostMessage& AddPosition(vec3 trans);
		GhostMessage& AddRotation(quat rot);
		GhostMessage& AddScale(vec3 scl);

		opt<vec3> GetPosition() const;
		opt<quat> GetRotation() const;
		opt<vec3> GetScale() const;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, network_id, 0, 4096);
			serialize_int(stream, state_mask, 0, std::numeric_limits<int>::max());

			if (state_mask & GhostFlags::TRANSFORM_POS)
			{
				serialize_float(stream, position.x);
				serialize_float(stream, position.y);
				serialize_float(stream, position.z);
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