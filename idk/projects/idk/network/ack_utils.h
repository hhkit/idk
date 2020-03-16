#pragma once
#include <idk.h>
#include <network/network.h>

namespace idk
{
	vector<SeqNo> ackfield_to_acks(SeqNo begin, unsigned mask);
	unsigned acks_to_ackfield(SeqNo base, span<SeqNo>);
}