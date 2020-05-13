#pragma once

#include <idk.h>

namespace idk
{
	struct Address
	{
		union
		{
			unsigned char nums[4];
			struct { unsigned char a, b, c, d; };
		};
		unsigned short port;
		constexpr Address()
			: a{}, b{}, c{}, d{}, port{} {}
		constexpr Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port = static_cast<unsigned short>(-1))
			: a{ a }, b{ b }, c{ c }, d{ d }, port{ port }
		{}

		explicit operator string() const;
		bool operator<(const Address& rhs) const;
	};

	constexpr auto localhost = Address{ 127,0,0,1,8000 };
}