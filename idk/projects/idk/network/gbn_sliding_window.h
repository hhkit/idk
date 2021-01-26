#pragma once
#include <deque>
#include <idk.h>
#include <network/SeqNo.h>

namespace idk
{
	template<typename T>
	class gbn_sliding_window
	{
	public:
		explicit gbn_sliding_window(seq_num_t first_message) : next_seq_expected_{ first_message } {}
		seq_num_t base() const { return buffer_.size() ? buffer_.front().sequence_number : next_seq_expected_; }
		seq_num_t expected() const { return next_seq_expected_; }
		template<typename ... Args>
		bool emplace(seq_num_t num, Args&& ... args);
		opt<T> pop_front();
		size_t size() const { return buffer_.size(); }
	private:
		struct Node
		{
			seq_num_t sequence_number;
			T value;
		};

		seq_num_t next_seq_expected_;
		std::deque<Node> buffer_;
	};
	template<typename T>
	template<typename ...Args>
	inline bool gbn_sliding_window<T>::emplace(seq_num_t num, Args&& ... args)
	{
		if (num < next_seq_expected_)
			return false;

		const auto insert = std::find_if(buffer_.begin(), buffer_.end(), [num](const Node& val)
		{
			return val.sequence_number >= num;
		});

		if (insert != buffer_.end())
		{
			if (insert->sequence_number == num)
				return false;
		}

		if (buffer_.empty())
			next_seq_expected_ = num + 1;
		buffer_.emplace(insert, Node{ num, std::forward<Args>(args)... });
		return true;
	}

	template<typename T>
	opt<T> gbn_sliding_window<T>::pop_front()
	{
		if (buffer_.empty())
			return std::nullopt;

		auto retval = std::move(buffer_.front());
		buffer_.pop_front();
		next_seq_expected_ = retval.sequence_number + 1;

		return std::move(retval.value);
	}
}  