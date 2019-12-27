#pragma once

namespace idk
{
	template<typename FwdIter, typename Container>
	struct range_over
	{
		Container c;
		FwdIter _begin;
		FwdIter _end;

		constexpr auto begin() noexcept { return _begin; }
		constexpr auto end() noexcept { return _end; }
	};

	template<typename Container> constexpr auto reverse(Container&& cont) noexcept;
}