#pragma once
#include <network/SeqNo.h>
namespace idk
{
	using NetworkID = unsigned;
	using SeqNo     = seq_num_t;
	using StateMask = unsigned;

	enum class Host : int
	{
		NONE = -2,
		SERVER = -1,
		CLIENT0 = 0,
		CLIENT1,
		CLIENT2,
		CLIENT3,
		CLIENT_MAX,
		ANY = CLIENT_MAX,
	};

	static constexpr SeqNo seq_max = SeqNo{ SeqNo::max_value };
	static constexpr auto sliding_window_buf = 30;

	bool seqno_greater_than(SeqNo lhs, SeqNo rhs);
}