#pragma once
#include <ds/small_string.h>
#include <network/Address.h>

namespace idk
{
	struct Device
	{
		idk::small_string<char> name;
		idk::small_string<char> fullname;
		vector<Address> ip_addresses;
	};
}