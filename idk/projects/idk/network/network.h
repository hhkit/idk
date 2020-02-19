#pragma once

namespace idk
{
	using NetworkID = unsigned;
	using SeqNo     = unsigned short;
	using StateMask = unsigned;

	enum class Host
	{
		ME = -2,
		SERVER = -1,
		CLIENT0 = 0,
		CLIENT1,
		CLIENT2,
		CLIENT3,
		CLIENT_MAX,
		ANY = CLIENT_MAX,
	};

	bool seqno_greater_than(SeqNo lhs, SeqNo rhs);
}