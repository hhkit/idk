#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/network.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventInvokeRPCMessage
		: public yojimbo::Message
	{
	public:
		struct Param
		{
			int size = 0;
			vector<unsigned char> buffer;
		};
		NetworkID invoke_on_id = 0;
		char method_name[64];
		unsigned param_count = 0;
		vector<Param> param_buffer;


		template <typename Stream>
		bool Serialize(Stream& stream)
		{
			serialize_int(stream, invoke_on_id, 0, 4096);
			serialize_string(stream, method_name, 64);
			serialize_uint32(stream, param_count);
			param_buffer.resize(param_count);

			for (auto& elem : param_buffer)
			{
				serialize_int(stream, elem.size, 0, 0x7FFF);
				elem.buffer.resize(elem.size);
				serialize_bytes(stream, (uint8_t*)elem.buffer.data(), elem.size);
			}

			return true;
		}

		YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
	};
}