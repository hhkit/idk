#include "stdafx.h"
#include "ack_utils.h"

namespace idk
{
	vector<SeqNo> ackfield_to_acks(SeqNo begin, unsigned mask)
	{
		vector<SeqNo> retval;
		while (mask)
		{
			if (mask & 1)
				retval.emplace_back(begin);

			--begin;
			mask >>= 1;
		}
		return retval;
	}

	unsigned acks_to_ackfield(SeqNo base, span<SeqNo> sequence_numbers)
	{
		unsigned mask{};

		for (auto& elem : sequence_numbers)
		{
			auto diff = base - elem;
			if (diff < 32)
				mask |= 1 << diff;
		}

		return mask;
	}

}