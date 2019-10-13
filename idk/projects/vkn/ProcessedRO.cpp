#include "ProcessedRO.h"
namespace idk::vkn
{

std::optional<vk::Buffer> ProcessedRO::BindingInfo::GetBuffer() const
{
	std::optional<vk::Buffer> ret;
	if (ubuffer.index() == meta::IndexOf<data_t, vk::Buffer>::value)
		ret = std::get<vk::Buffer>(ubuffer);
	return ret;
}

}