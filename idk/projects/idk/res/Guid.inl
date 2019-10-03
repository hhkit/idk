#pragma once

namespace idk
{
	constexpr Guid::Guid() noexcept
		: Data1{ 0 }, Data2{ 0 }, Data3{ 0 }, Data4{ 0,0,0,0,0,0,0,0 }
	{}

	constexpr Guid::Guid(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
		: Data1{ a }
		, Data2{ unsigned short (b >> 2 & 0xFFFF) }
		, Data3{ unsigned short (b      & 0xFFFF) }
		, Data4{
			(unsigned char) (c >> 24 & 0xFF)
		,	(unsigned char) (c >> 16 & 0xFF)
		,	(unsigned char) (c >>  8 & 0xFF)
		,	(unsigned char) (c >>  0 & 0xFF)
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