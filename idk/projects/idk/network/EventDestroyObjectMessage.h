#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventDestroyObjectMessage
		: public yojimbo::Message
	{
	public:
		NetworkID id;

		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, id, 0, 4096);
			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};

}