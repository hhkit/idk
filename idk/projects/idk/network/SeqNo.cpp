#include "stdafx.h"
#include "SeqNo.h"
#include <stdexcept>
#include <ostream>

namespace idk
{
	seq_num_t& seq_num_t::operator++()
	{
		if (value == max_value)
			value = 0;
		else
			++value;
		return *this;
	}

	seq_num_t seq_num_t::operator++(int)
	{
		auto copy = *this;
		++(*this);
		return copy;
	}

	seq_num_t& seq_num_t::operator--()
	{
		if (value == 0)
			value = max_value;
		else
			--value;

		return *this;
	}

	seq_num_t seq_num_t::operator--(int)
	{
		auto copy = *this;
		--(*this);
		return copy;
	}

	std::ostream& operator<<(std::ostream& stream, seq_num_t val)
	{
		auto flags = stream.flags();
		stream << std::hex << val.value;
		stream.flags(flags);
		return stream;
	}

	seq_num_t operator+(seq_num_t lhs, seq_num_t::difference_type rhs)
	{
#ifdef _DEBUG
		if (abs(rhs) > seq_num_t::max_value)
			throw std::out_of_range("rhs is higher than the wrap around, undefined behavior");
#endif
		if (rhs > 0)
		{
			const auto until_lhs_wraps_around = seq_num_t::max_value - lhs.value;
			const auto positive_rhs = static_cast<seq_num_t::value_type>(rhs);
			if (until_lhs_wraps_around > positive_rhs)
				return seq_num_t{ lhs.value + positive_rhs };
			else
				return seq_num_t{ positive_rhs - until_lhs_wraps_around };
		}
		else
		{
			// less than zero
			const auto displacement = static_cast<seq_num_t::value_type>(-rhs);
			return displacement > lhs.value ? seq_num_t{ seq_num_t::max_value - (displacement - lhs.value) + 1 } : seq_num_t{ lhs.value - displacement };
		}
	}

	seq_num_t operator-(seq_num_t lhs, seq_num_t::difference_type rhs)
	{
		return lhs + -rhs;
	}

	seq_num_t::difference_type operator-(seq_num_t lhs, seq_num_t rhs)
	{
		if (lhs.value > rhs.value)
		{
			auto diff = lhs.value - rhs.value;
			return diff > seq_num_t::max_value ? -static_cast<seq_num_t::difference_type>(seq_num_t::max_value - diff) : diff;
		}
		else // rhs > lhs
		{
			auto diff = rhs.value - lhs.value;
			return diff > seq_num_t::max_value ? -static_cast<seq_num_t::difference_type>(seq_num_t::max_value - diff) : diff;
		}
	}

	bool operator<(seq_num_t lhs, seq_num_t rhs)
	{
		return rhs > lhs;
	}

	bool operator>(seq_num_t lhs, seq_num_t rhs) 
	{
		constexpr auto half_wrap = seq_num_t::value_type((seq_num_t::max_value >> 1) + 1); // RANGE = HIGH - LOW + 1
		return (lhs.value > rhs.value) && (lhs.value - rhs.value <= half_wrap) 
			|| (lhs.value < rhs.value) && (rhs.value - lhs.value  > half_wrap);
	}

	bool operator<=(seq_num_t lhs, seq_num_t rhs) 
	{
		return !(lhs > rhs);
	}

	bool operator>=(seq_num_t lhs, seq_num_t rhs)
	{
		return !(rhs > lhs);
	}

	bool operator==(seq_num_t lhs, seq_num_t rhs)
	{
		return lhs.value == rhs.value;
	}

	bool operator!=(seq_num_t lhs, seq_num_t rhs)
	{
		return lhs.value != rhs.value;
	}
}