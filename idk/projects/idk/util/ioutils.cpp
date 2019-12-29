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

    string binarify(std::istream& istream)
    {
        istream.seekg(0, std::ios::end);
        std::streamsize size = istream.tellg();
        istream.seekg(0, std::ios::beg);

        string buffer;
        buffer.resize(size);
        istream.read(buffer.data(), size);
        return buffer;
    }

}