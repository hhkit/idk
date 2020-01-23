#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventInstantiatePrefabMessage
		: public yojimbo::Message
	{
	public:
		NetworkID id = 0;

		RscHandle<class Prefab> prefab;
		bool use_position;
		vec3 position;
		bool use_rotation;
		quat rotation;

		template <typename Stream>
		bool Serialize(Stream& stream) 
		{
			serialize_bytes(stream, (uint8_t*) &prefab, sizeof(prefab));
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
				serialize_float(stream, rotation.x);
				serialize_float(stream, rotation.y);
				serialize_float(stream, rotation.z);
				serialize_float(stream, rotation.w);
			}
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}