#pragma once
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <utility>

namespace idk
{
	struct seq_num_t
	{
		using size_type = uint32_t;
		using value_type = uint32_t;
		using difference_type = int32_t;

		static constexpr value_type max_value = 0xFFFF; 

		value_type value;

		explicit constexpr seq_num_t(value_type number = {}) noexcept : value{ number } {}

		seq_num_t& operator++();
		seq_num_t  operator++(int);
		seq_num_t& operator--();
		seq_num_t  operator--(int);
	};

	std::ostream& operator<<(std::ostream& stream, seq_num_t val);

	seq_num_t operator+(seq_num_t lhs, seq_num_t::difference_type rhs);
	seq_num_t operator-(seq_num_t lhs, seq_num_t::difference_type rhs);
	seq_num_t::difference_type operator-(seq_num_t lhs, seq_num_t rhs);

	bool operator< (seq_num_t lhs, seq_num_t rhs);
	bool operator> (seq_num_t lhs, seq_num_t rhs);
	bool operator<=(seq_num_t lhs, seq_num_t rhs);
	bool operator>=(seq_num_t lhs, seq_num_t rhs);
	bool operator==(seq_num_t lhs, seq_num_t rhs);
	bool operator!=(seq_num_t lhs, seq_num_t rhs);
}

namespace std
{
	template<>
	struct hash<idk::seq_num_t>
	{
		uint64_t operator()(idk::seq_num_t seq) const
		{
			return std::hash<idk::seq_num_t::value_type>{}(seq.value);
		}
	};
}