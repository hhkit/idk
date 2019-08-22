#pragma once
#include <vkn/BufferHelpers.h>
template<typename T>
std::pair<vk::Buffer, uint32_t> idk::vkn::UboManager::Add(const T& data)
{
	auto size = hlp::buffer_size(data);
	DataPair& pair = find_pair(size);
	return std::make_pair(pair.Buffer(), pair.Add(size, std::data(data)));
}