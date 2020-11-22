#pragma once
#include <idk.h>
#include <scene/Scene.h>
#include <network/network.h>
#include <network/Message.h>

namespace idk
{
	class EventDataBlockBufferedEvents
		: public Message
	{
	public:
		struct ObjectAndID
		{
			Handle<GameObject> handle;
			NetworkID          id;
			vec3               position;
			quat               rotation;
			bool               destroyed{ false };
		};

		struct PrefabAndID
		{
			RscHandle<Prefab> prefab;
			NetworkID         id;
			vec3              position;
			quat              rotation;
		};

		RscHandle<Scene> load_scene;
		vector<ObjectAndID> loaded_objects;
		vector<PrefabAndID> loaded_prefabs;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_dataonly_struct(stream, load_scene);
			serialize_vector_count(stream, loaded_objects, 4096);

			for (auto& elem : loaded_objects)
			{
				serialize_dataonly_struct(stream, elem.handle);
				serialize_bool(stream, elem.destroyed);
				if (!elem.destroyed)
				{
					serialize_int(stream, elem.id, 0, 4096);
					serialize_vec(stream, elem.position);
					serialize_vec(stream, elem.rotation);
				}
			}

			serialize_vector_count(stream, loaded_prefabs, 4096);
			for (auto& elem : loaded_prefabs)
			{
				serialize_dataonly_struct(stream, elem.prefab);
				serialize_int(stream, elem.id, 0, 4096);
				serialize_vec(stream, elem.position);
				serialize_vec(stream, elem.rotation);
			}

			return true;
		}

		NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()
	};
}