#pragma once
#include <idk.h>
#include <iosfwd>

namespace idk
{
	string stringify(const std::istream& istream);
    string binarify(std::istream& istream);
}