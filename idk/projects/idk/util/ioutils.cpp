#include "stdafx.h"
#include "ioutils.h"
#include <sstream>

namespace idk
{
	string stringify(const std::istream& istream)
	{
		std::stringstream s;
		s << istream.rdbuf();
		return s.str();
	}
}