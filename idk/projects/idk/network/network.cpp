#include "stdafx.h"
#include "network.h"

bool idk::seqno_greater_than(SeqNo s1, SeqNo s2)
{
    return
        ((s1 > s2) && (s1 - s2 <= 32768)) ||
        ((s1 < s2) && (s2 - s1 > 32768));
}
