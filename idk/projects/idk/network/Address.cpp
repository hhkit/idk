#include "stdafx.h"
#include "Address.h"

namespace idk
{
	Address::operator string() const
	{
		char buf[32];
		if (port != static_cast<unsigned short>(-1))
			sprintf_s(buf, "%d.%d.%d.%d:%d", a, b, c, d, port);
		else
			sprintf_s(buf, "%d.%d.%d.%d", a, b, c, d);
		return buf;
	}
	bool Address::operator<(const Address& rhs) const
	{
		for (int i = 0; i < 4; ++i)
		{
			if (nums[i] < rhs.nums[i])
				return true;
			if (nums[i] > rhs.nums[i])
				return false;
		}

		return port < rhs.port;
	}
}