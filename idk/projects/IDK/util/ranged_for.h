#pragma once

namespace idk
{
	template<typename FwdIter, typename Container>
	struct range_over
	{
		Container c;
		FwdIter _begin;
		FwdIter _end;

		auto begin() { return _begin; }
		auto end() { return _end; }
	};

	template<typename Container> auto reverse(Container&& cont);
}
#include "ranged_for.inl"