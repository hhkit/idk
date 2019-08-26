#include "pch.h"
#pragma once
#include <iosfwd>

namespace idk::vkn::hlp {

	std::ostream& cerr();

	std::string GetBinaryFile(const std::string& filepath);
}