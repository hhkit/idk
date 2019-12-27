#pragma once

#include "Guid.h"

namespace idk
{
	constexpr Guid::Guid(unsigned int a, unsigned short b, unsigned short c, unsigned long long d)
		: Data1{ a }
		, Data2{ b }
		, Data3{ c }
		, Data4{
			(unsigned char) (d >> 56 & 0xFF)
		,	(unsigned char) (d >> 48 & 0xFF)
		,	(unsigned char) (d >> 40 & 0xFF)
		,	(unsigned char) (d >> 32 & 0xFF)
		,	(unsigned char) (d >> 24 & 0xFF)
		,	(unsigned char) (d >> 16 & 0xFF)
		,	(unsigned char) (d >>  8 & 0xFF)
		,	(unsigned char) (d >>  0 & 0xFF)
	}
	{}

	inline Guid Guid::Make()
	{
		Guid guid;
		const auto res = CoCreateGuid((GUID*)& guid); (res);
		return guid;
	}
}