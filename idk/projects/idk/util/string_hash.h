/******************************************************************************/
/*!
\project	September
\file		StringHash.h
\author		Ho Han Kit Ivan
\brief
	Compile time hashing of strings.
	See: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

All content (C) 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#pragma warning (disable:4307)
#pragma warning (disable:26450)
#include <string_view>

namespace idk
{
	constexpr size_t string_hash(std::string_view str) noexcept;
}

