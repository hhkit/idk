#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <core/Handle.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventLoadLevelMessage
		: public yojimbo::Message
	{
	public:
		struct ViewMapping
		{
			Handle<ElectronView> view;
			NetworkID id;
		};

		RscHandle<Scene> GetScene() const { return scene; }
		span<const ViewMapping> GetObjects() const { return obj_list; }
		EventLoadLevelMessage& SetScene(RscHandle<Scene>);
		EventLoadLevelMessage& AddView(Handle<ElectronView>);

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_bytes(stream, (uint8_t*)&scene, sizeof(scene));
			serialize_int(stream, obj_count, 0, 4096);
			obj_list.resize(obj_count);
			for (auto& elem : obj_list)
			{
				serialize_uint64(stream, elem.view.id);
				serialize_int(stream, elem.id, 0, 4096);
			}
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	private:

		RscHandle<Scene> scene;
		unsigned obj_count{};
		vector<ViewMapping> obj_list;
	};
}