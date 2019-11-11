#include <pch.h>
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

std::optional<ProcessedRO::image_t> ProcessedRO::BindingInfo::GetImage() const
{
	using Type = image_t;
	std::optional<Type> ret;
	if (IsImage())
		ret = std::get<Type>(ubuffer);
	return ret;
}

bool ProcessedRO::BindingInfo::IsImage() const
{
	using Type = image_t;
	return ubuffer.index() == meta::IndexOf<data_t, Type>::value;
}

vk::DescriptorSetLayout ProcessedRO::BindingInfo::GetLayout() const
{
	return layout;
}

}