#include "stdafx.h"
#include <res/Guid.h>

namespace idk
{
	Guid::Guid(const std::string_view& str) noexcept
		: Guid{}
	{
		if (sscanf_s(str.data(),
			"%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
			&Data1, &Data2, &Data3,
			&Data4[0], &Data4[1], &Data4[2], &Data4[3],
			&Data4[4], &Data4[5], &Data4[6], &Data4[7]) != 11)
			*this = Guid::Make();
	}

	bool Guid::operator==(const Guid& other) const noexcept
	{
		return std::memcmp(this, &other, sizeof(GUID)) == 0;
	}

	Guid::operator string() const
	{
		char guid_cstr[39];
		snprintf(guid_cstr, sizeof(guid_cstr),
			"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			Data1, Data2, Data3,
			Data4[0], Data4[1], Data4[2], Data4[3],
			Data4[4], Data4[5], Data4[6], Data4[7]);
		return string{ guid_cstr };
	}

	Guid::operator bool() const noexcept
	{
		return *reinterpret_cast<const GUID*>(this) != GUID_NULL;
	}
}