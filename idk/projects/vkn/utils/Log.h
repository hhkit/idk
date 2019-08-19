#include "pch.h"
#pragma once
#include <iosfwd>

namespace idk::vkn {

	namespace utl
	{
		std::ostream& cerr();
	}

	std::string GetBinaryFile(const std::string& filepath);
}