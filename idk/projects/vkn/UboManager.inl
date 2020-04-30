#pragma once
#include "UboManager.h"
#include <vkn/BufferHelpers.inl>

namespace idk::vkn
{

	template<typename T>
	std::pair<vk::Buffer, uint32_t> UboManager::Add(const T& data)
	{
		auto size = hlp::buffer_size(data);
		DataPair& pair = FindPair(size);
		return make_buffer_pair(pair, size, hlp::buffer_data(data));
	}
}