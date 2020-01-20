#pragma once
#include <yojimbo/yojimbo.h>
#include <network/events/InstantiatePrefabPayload.h>

namespace idk
{
	class EventInstantiatePrefabMessage : public yojimbo::Message {
	public:
		EventInstantiatePrefabPayload payload;

		EventInstantiatePrefabMessage() = default;

		template <typename Stream>
		bool Serialize(Stream& stream) 
		{
			serialize_int(stream, payload.view_id, 0, 4096);
			serialize_bytes(stream, reinterpret_cast<uint8_t*>(&payload.prefab), sizeof(payload.prefab));
			serialize_bool(stream, payload.has_position);
			if (payload.has_position)
			{
				serialize_float(stream, payload.position.x);
				serialize_float(stream, payload.position.y);
				serialize_float(stream, payload.position.z);
			}
			serialize_bool(stream, payload.has_rotation);
			if (payload.has_rotation)
			{
				serialize_float(stream, payload.rotation.x);
				serialize_float(stream, payload.rotation.y);
				serialize_float(stream, payload.rotation.z);
				serialize_float(stream, payload.rotation.w);
			}
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}