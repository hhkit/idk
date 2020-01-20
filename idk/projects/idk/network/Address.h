#pragma once

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
		constexpr Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
			: a{ a }, b{ b }, c{ c }, d{ d }, port{ port }
		{}
	};

	constexpr auto localhost = Address{ 127,0,0,1,8000 };
}