#pragma once
#include "UboManager.h"
#include <vkn/BufferHelpers.inl>
#include <vkn/Stopwatch.h>
namespace idk::vkn
{

	namespace dbg
	{
		hash_table<string_view, float>& get_rendertask_durations();
		void add_rendertask_durations(string_view name, float duration);

	}
	using dbg::add_rendertask_durations;
	template<typename T>
	std::pair<vk::Buffer, uint32_t> UboManager::Add(const T& data)
	{
		dbg::stopwatch timer;
		timer.start();
		auto size = hlp::buffer_size(data);
		DataPair& pair = FindPair(size);
		timer.stop();
		add_rendertask_durations("Ubo Add-FindPair", timer.time().count());
		return make_buffer_pair(pair, size, hlp::buffer_data(data));
	}
}