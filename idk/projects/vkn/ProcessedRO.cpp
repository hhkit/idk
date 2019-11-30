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

std::optional<ProcessedRO::AttachmentBinding> ProcessedRO::BindingInfo::GetAttachment() const
{
	using Type = AttachmentBinding;
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

bool ProcessedRO::BindingInfo::IsAttachment() const
{
	using Type = AttachmentBinding;
	return ubuffer.index() == meta::IndexOf<data_t, Type>::value;
}

vk::DescriptorSetLayout ProcessedRO::BindingInfo::GetLayout() const
{
	return layout;
}

const RenderObject& ProcessedRO::Object() const
{
	return *itr;
}

const std::optional<vk::DescriptorSet>& ProcessedRO::GetDescriptorSet(uint32_t set)const
{
	return descriptor_sets[set];
}

void ProcessedRO::SetDescriptorSet(uint32_t set, vk::DescriptorSet ds)
{
	if (set >= descriptor_sets.size())
		descriptor_sets.resize(set * 2);
	descriptor_sets[set] = ds;
}
/*
ProcessedRO::ProcessedRO(
	const RenderObject*                       itr_            , 
	hash_table<uint32_t, vector<BindingInfo>> bindings_       ,
	shared_ptr<pipeline_config>               config_         , 
	std::optional<RscHandle<ShaderProgram>>   vertex_shader_  , 
	std::optional<RscHandle<ShaderProgram>>   geom_shader_    , 
	std::optional<RscHandle<ShaderProgram>>   frag_shader_    , 
	vector<std::optional<vk::DescriptorSet>>  descriptor_sets_,
	bool                                      rebind_shaders_ , 
	size_t                                    num_instances_  , 
	size_t                                    inst_offset_    ):
	itr			     {std::move(itr_            )},
	bindings		 {std::move(bindings_       )},
	config			 {std::move(config_         )},
	vertex_shader	 {std::move(vertex_shader_  )},
	geom_shader	     {std::move(geom_shader_    )},
	frag_shader	     {std::move(frag_shader_    )},
	descriptor_sets  {std::move(descriptor_sets_)},
	rebind_shaders   {std::move(rebind_shaders_ )},
	num_instances    {std::move(num_instances_  )},
	inst_offset      {std::move(inst_offset_    )}
{
}
*/
}