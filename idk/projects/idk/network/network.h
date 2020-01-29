#pragma once

namespace idk
{
	using NetworkID = unsigned;

	enum class Host
	{
		SERVER = -1,
		CLIENT0 = 0,
		CLIENT1,
		CLIENT2,
		CLIENT3,
		CLIENT_MAX,
		ANY = CLIENT_MAX,
	};
}