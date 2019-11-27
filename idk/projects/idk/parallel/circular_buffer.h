#pragma once
#include <atomic>
#include <optional>

#include <parallel/cache_aligned_wrapper.h>
namespace idk::mt
{
	template<typename T, size_t N = 1024>
	class circular_buffer_lf
	{
	public:
		circular_buffer_lf();
		~circular_buffer_lf();

		template<typename ... Args>
		bool emplace_back(Args&& ... args);
		std::optional<T> pop_front();
	private:
		struct Node;

		alignas(machine::cache_line_sz) std::atomic<size_t> head{};
		alignas(machine::cache_line_sz) std::atomic<size_t> tail{};

		cache_aligned_wrapper<Node> buffer[N + 1];
	};

	template<typename T, size_t N>
	struct circular_buffer_lf<T, N>::Node 
	{
		std::atomic<size_t> next_node{};
		std::optional<T> data;
	};

	template<typename T, size_t N>
	inline circular_buffer_lf<T, N>::circular_buffer_lf()
	{
		for (size_t i = 0; i < N; ++i)
			buffer[i].value.next_node = i;
	}

	template<typename T, size_t N>
	inline circular_buffer_lf<T, N>::~circular_buffer_lf()
	{
		
	}

	template<typename T, size_t N>
	template<typename ...Args>
	inline bool circular_buffer_lf<T, N>::emplace_back(Args&& ...args)
	{
		// grab current tail
		auto ltail = tail.load(std::memory_order_relaxed);
		while (true)
		{
			auto& slot = buffer[ltail % N].value;

			auto next_tail = slot.next_node.load(std::memory_order_acquire);
			auto diff = static_cast<long long>(next_tail) - static_cast<long long>(ltail);

			if (diff == 0) // valid push_back
			{
				if (tail.compare_exchange_weak(ltail, ltail + 1))
				{
					slot.data.emplace(std::forward<Args>(args)...);
					slot.next_node.store(ltail + 1, std::memory_order_release);
					return true;
				}
			}

			if (diff < 0) // full queue
				return false;

			ltail = tail.load(std::memory_order_relaxed);
		}
	}

	template<typename T, size_t N>
	std::optional<T> circular_buffer_lf<T, N>::pop_front()
	{
		auto lhead = head.load(std::memory_order_relaxed);
		while (true)
		{
			auto& slot = buffer[lhead % N].value;
			auto next_tail = slot.next_node.load(std::memory_order_acquire);
			auto diff = static_cast<long long>(next_tail) - static_cast<long long>(head + 1);

			if (diff == 0) // valid head
			{
				if (head.compare_exchange_weak(lhead, lhead + 1, std::memory_order_relaxed))
				{
					auto retval = std::move(slot.data);
					slot.data.reset();
					slot.next_node.store(lhead + N, std::memory_order_release);
					return retval;
				}
			}

			if (diff < 0)
				return std::nullopt;

			lhead = head.load(std::memory_order_relaxed);
		}
	}
}