#pragma once
#include "UboManager.h"
#include <vkn/BufferHelpers.inl>
template<typename T>
std::pair<vk::Buffer, uint32_t> idk::vkn::UboManager::Add(const T& data)
{
	auto size = hlp::buffer_size(data);
	DataPair& pair = FindPair(size);
	return std::make_pair(pair.Buffer(), pair.Add(size, hlp::buffer_data(data)));
}