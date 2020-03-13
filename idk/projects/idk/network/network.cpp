#include "stdafx.h"
#include "network.h"
#include <yojimbo/yojimbo.h>
bool idk::seqno_greater_than(SeqNo lhs, SeqNo rhs)
{
	return yojimbo::sequence_greater_than(lhs.value, rhs.value);
}
