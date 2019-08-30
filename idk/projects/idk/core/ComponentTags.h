#pragma once
#include <meta/tag.h>

namespace idk
{
	template<typename ... Ts>
	struct RequireComponent
	{
		using Required = std::tuple<Ts ...>;
	};
}