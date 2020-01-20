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
}