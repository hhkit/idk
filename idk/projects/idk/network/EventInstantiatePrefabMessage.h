#pragma once
#include <idk.h>
#include <network/network.h>
#include <network/Message.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventInstantiatePrefabMessage
		: public Message
	{
	public:
		NetworkID id = 0;

		RscHandle<class Prefab> prefab;
		bool use_position;
		vec3 position;
		bool use_rotation;
		float rotation[4];

		template <typename Stream>
		bool Serialize(Stream& stream) 
		{
			serialize_bytes(stream, (uint8_t*) &prefab, sizeof(prefab));
			serialize_int(stream, id, 0, 4096);
			serialize_bool(stream, use_position);
			if (use_position)
			{
				serialize_float(stream, position.x);
				serialize_float(stream, position.y);
				serialize_float(stream, position.z);
			}
			serialize_bool(stream, use_rotation);
			if (use_rotation)
			{
				serialize_float(stream, rotation[0]);
				serialize_float(stream, rotation[1]);
				serialize_float(stream, rotation[2]);
				serialize_float(stream, rotation[3]);
			}
			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};
}