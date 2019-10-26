#include "pch.h"
#include "ShaderModule.h"
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <spirv_cross/spirv_reflect.hpp>
#include <gfx/pipeline_config.h>
#include <vkn/GfxConverters.h>
namespace idk::vkn
{
	namespace spx = spirv_cross;
	uniform_layout_t::UniformStage StageToUniformStage(vk::ShaderStageFlagBits single_stage)
	{
		uniform_layout_t::UniformStage result;
		switch (single_stage)
		{
		case vk::ShaderStageFlagBits::eVertex:
			result = uniform_layout_t::UniformStage::eVertex;
			break;
		case vk::ShaderStageFlagBits::eFragment:
			result = uniform_layout_t::UniformStage::eFragment;
			break;
		case vk::ShaderStageFlagBits::eTessellationControl:
			result = uniform_layout_t::UniformStage::eTessCtrl;
			break;
		case vk::ShaderStageFlagBits::eTessellationEvaluation:
			result = uniform_layout_t::UniformStage::eTessEval;
			break;
		case vk::ShaderStageFlagBits::eGeometry:
			result = uniform_layout_t::UniformStage::eGeometry;
			break;
		case vk::ShaderStageFlagBits::eCompute:
			result = uniform_layout_t::UniformStage::eCompute;
			break;
		default:
			throw std::runtime_error("Unexpected shader stage encountered.");
			break;
		}
		return result;
	}

	//uint32_t get_struct_size(spx::CompilerReflection& code_reflector, spirv_cross::SPIRType type)
	//{
	//	for (auto& member_type : type.member_types)
	//	{
	//		auto tmp = code_reflector.get_type(member_type);
	//		info.size += s_cast<uint32_t>(code_reflector.get_declared_struct_member_size(type, i));
	//	}
	//}
	AttribFormat GetFormat(const spirv_cross::SPIRType& type)
	{
		static AttribFormat vec_formats[5]
		{
			AttribFormat{},
			AttribFormat::eSVec1,
			AttribFormat::eSVec2,
			AttribFormat::eSVec3,
			AttribFormat::eSVec4,
		};
		static AttribFormat ivec_formats[5]
		{
			AttribFormat{},
			AttribFormat::eIVec1,
			AttribFormat::eIVec2,
			AttribFormat::eIVec3,
			AttribFormat::eIVec4,
		};
		AttribFormat format{};
		switch (type.basetype)
		{
		case spirv_cross::SPIRType::Float:
			format = vec_formats[type.vecsize];
			break;
		case spirv_cross::SPIRType::Int:
			format = ivec_formats[type.vecsize];
			break;
		}
		return format;
	}
	VertexRate GetRate(string var_name)
	{
		static const hash_table<string, VertexRate> map
		{
			{"positon"         ,VertexRate::eVertex},
			{"normal"          ,VertexRate::eVertex},
			{"uv"              ,VertexRate::eVertex},
			{"tangent"         ,VertexRate::eVertex},
			{"bone_ids"        ,VertexRate::eVertex},
			{"bone_weights"    ,VertexRate::eVertex},
		};
		VertexRate result{};
		auto itr = map.find(var_name);
		result = (itr != map.end()) ? itr->second : result;
		return result;
	}
	struct ExtractedMisc
	{
		//location
		hash_table<uint32_t,buffer_desc> extracted_desc;
	};
	ExtractedMisc extract_info(const vector<unsigned int>& buffer, hash_table<string, UboInfo>& ubo_info, vk::ShaderStageFlagBits single_stage)
	{
		ExtractedMisc result{};
		spx::CompilerReflection code_reflector{ buffer };// r_cast<const uint32_t*>(std::data(buffer)), byte_code.size() / sizeof(uint16_t)//};
		//code_reflector.build_combined_image_samplers();
		auto aaaa = code_reflector.compile();
		auto resources = code_reflector.get_shader_resources();
		//type
		
		ubo_info.clear();
		struct tmp_t
		{
			spirv_cross::SPIRType base_type;
			spirv_cross::SPIRType type;
			string name;
			uint32_t id;
		};
		if (single_stage == vk::ShaderStageFlagBits::eVertex)
		{

		for (auto&& input : resources.stage_inputs)
		{
			auto tmp = tmp_t{ code_reflector.get_type(input.base_type_id),code_reflector.get_type(input.type_id),input.name,input.id };// temps.emplace_back();
			auto location = code_reflector.get_decoration(input.id, spv::Decoration::DecorationLocation);
			auto& bdesc = result.extracted_desc[location];// .desc;
			bdesc.AddAttribute(GetFormat(tmp.type), location, 0,true);
			//bdesc.binding.binding_index = 0;
			bdesc.binding.stride = (tmp.type.width* tmp.type.vecsize)/8;
			bdesc.binding.vertex_rate = GetRate(tmp.name);
		}
		}
		for (auto& ub : resources.uniform_buffers)
		{
			UboInfo info;
			auto type = code_reflector.get_type(ub.type_id);
			info.binding = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationBinding);
			info.set = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationDescriptorSet);
			info.stage = StageToUniformStage(single_stage);
			info.size = type.width;
			info.type = uniform_layout_t::UniformType::eBuffer;
			uint32_t i = 0;
			for (auto& member_type : type.member_types)
			{
				auto tmp = code_reflector.get_type(member_type);
				info.size += s_cast<uint32_t>(code_reflector.get_declared_struct_member_size(type, i++));
			}
			ubo_info[ub.name] = std::move(info);
		}
		//auto& cs = code_reflector.get_combined_image_samplers();

		for (auto& ub : resources.sampled_images)
		{
			UboInfo info;
			auto type = code_reflector.get_type(ub.type_id);
			info.binding = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationBinding);
			info.set = code_reflector.get_decoration(ub.id, spv::Decoration::DecorationDescriptorSet);
			info.stage = StageToUniformStage(single_stage);
			info.size = (type.array.size())?type.array.back():1;
			info.type = uniform_layout_t::UniformType::eSampler;
			
			uint32_t i = 0;
			for (auto& member_type : type.member_types)
			{
				auto tmp = code_reflector.get_type(member_type);
				info.size += s_cast<uint32_t>(code_reflector.get_declared_struct_member_size(type, i));
			}
			ubo_info[ub.name] = std::move(info);
		}
		//for (auto& ub : cs)
		//{
		//	UboInfo info;
		//	
		//	info.binding = code_reflector.get_decoration(ub.combined_id, spv::Decoration::DecorationBinding);
		//	info.binding = code_reflector.get_decoration(ub.sampler_id, spv::Decoration::DecorationBinding);
		//	info.set = code_reflector.get_decoration(ub.combined_id, spv::Decoration::DecorationDescriptorSet);
		//	info.set = code_reflector.get_decoration(ub.sampler_id, spv::Decoration::DecorationDescriptorSet);
		//	info.stage = StageToUniformStage(single_stage);
		//	info.type = vk::DescriptorType::eCombinedImageSampler;
		//	uint32_t i = 0;
		//	ubo_info[code_reflector.get_name(ub.combined_id)] = std::move(info);
		//}
		return result;
	}

	vk::DescriptorType ConvertUniformType(uniform_layout_t::UniformType type)
	{
		static hash_table< uniform_layout_t::UniformType, vk::DescriptorType>map
		{
			{uniform_layout_t::UniformType::eBuffer,vk::DescriptorType::eUniformBuffer},
			{uniform_layout_t::UniformType::eSampler,vk::DescriptorType::eCombinedImageSampler},
		};
		return map[type];
	}
	vector<vk::DescriptorSetLayoutBinding> GetDescriptorBindings(const vector<uniform_layout_t::bindings_t>& ulayout_config)
	{
		vector<vk::DescriptorSetLayoutBinding> result;
		for (auto& binding : ulayout_config)
		{
			vk::DescriptorSetLayoutBinding a{
				binding.binding,
				ConvertUniformType(binding.type),
				binding.descriptor_count,
				hlp::MapStages(binding.stages)
				,nullptr
			};
			result.emplace_back(a);
		}
		return result;
	}
	void CreateLayouts(
		hash_table<string, UboInfo>& ubo_info,
		hash_table<uint32_t, vk::UniqueDescriptorSetLayout>& layouts,
		VulkanView& view)
	{

		hash_table<uint32_t, vector<uniform_layout_t::bindings_t>> set_layout;
		uint32_t max = 0;
		if (ubo_info.size())
		{
			for ([[maybe_unused]]auto& [set_name, info] : ubo_info)
			{
			
				max = std::max(info.set, max);
				set_layout[info.set].emplace_back(uniform_layout_t::bindings_t{ info.binding,(info.type!=uniform_layout_t::UniformType::eSampler)?1:info.size,{info.stage},info.type });
			}
			++max;
		}
		layouts.clear();
		//for (uint32_t i = 0; i < max; ++i)
		//{
		//	vk::DescriptorSetLayoutCreateInfo layout_info
		//	{
		//		vk::DescriptorSetLayoutCreateFlags{}
		//		,0
		//		,nullptr
		//	};
		//	layouts[i] = std::make_pair(false,view.Device()->createDescriptorSetLayoutUnique(layout_info, nullptr, view.Dispatcher()));
		//}
		for (auto& [set_idx, set] : set_layout)
		{
			auto bindings = GetDescriptorBindings(set);
			if (bindings.size())
			{
				vk::DescriptorSetLayoutCreateInfo layout_info
				{
					vk::DescriptorSetLayoutCreateFlags{}
					,hlp::arr_count(bindings)
					,hlp::arr_count(bindings) ? std::data(bindings) : nullptr
				};
				layouts[set_idx] = view.Device()->createDescriptorSetLayoutUnique(layout_info, nullptr, view.Dispatcher());
			}
		}
		for ([[maybe_unused]] auto& [set_name, info] : ubo_info)
		{
			
			info.layout = *layouts[info.set];
		}
	}
	void FillDefaultBufferDesc(ExtractedMisc& info, vector<buffer_desc>& descriptors)
	{
		vector<decltype(&*info.extracted_desc.begin())> pair_ptrs(info.extracted_desc.size());
		vector<bool> filled_bindings(descriptors.size()+info.extracted_desc.size(),false);
		auto get_next_binding = [](auto& filled_bindings) ->uint32_t
		{
			for (size_t i = 0; i < filled_bindings.size(); ++i)
			{
				if (!filled_bindings[i])
				{
					filled_bindings[i] = true;
					return s_cast<uint32_t>(i);
				}
			}
			throw std::runtime_error("incorrect binding setup");
            return static_cast<uint32_t>(-1);
		};
		pair_ptrs.resize(0);
		bool binding_set = false;
		for (auto& desc : descriptors)
		{
			if (desc.binding.binding_index)
				binding_set = filled_bindings[*desc.binding.binding_index] = true;
		}
		for (auto& pair : info.extracted_desc)
		{
			auto& location = pair.first;
			auto found = std::find_if(descriptors.begin(), descriptors.end(), 
				[&location](const buffer_desc& desc) 
				{
					bool found = false; 
					for (auto& attr : desc.attributes) 
					{
						found |= attr.location == location; 
					}
					return found;
				});
			if(found==descriptors.end())
				pair_ptrs.emplace_back(&pair);
		}
		for (auto& pair_ptr : pair_ptrs)
		{
			auto& desc = pair_ptr->second;
			if (binding_set)
				desc.binding.binding_index = get_next_binding(filled_bindings);
			descriptors.emplace_back(std::move(desc));
		}
		info.extracted_desc.clear();//invalid now
	}

void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, const vector<unsigned int>& buffer)
{
	string_view byte_code{r_cast<const char*>(buffer.data()),hlp::buffer_size(buffer)};
	auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
	auto back = std::make_unique<Data>();
	back->module = view.CreateShaderModule(byte_code);
	auto&& extracted= extract_info(buffer, back->ubo_info, single_stage);
	CreateLayouts(back->ubo_info,back->layouts,view);
	FillDefaultBufferDesc(extracted,descriptors);
	back->stage = single_stage;
	back->attrib_descriptions = std::move(descriptors);
	buf_obj.WriteToBack(std::move(back));
}
void ShaderModule::Load(vk::ShaderStageFlagBits single_stage, vector<buffer_desc> descriptors, string_view byte_code)
{
	vector<unsigned int> buffer;
	buffer.resize(byte_code.size()/sizeof(uint32_t));
	std::memcpy(buffer.data(), byte_code.data(), byte_code.size());
	Load(single_stage, descriptors, buffer);
}

bool ShaderModule::HasLayout(const string& uniform_name) const
{
	auto itr = Current().ubo_info.find(uniform_name);
	return itr != Current().ubo_info.end();;
}

hash_table<string, UboInfo>::const_iterator ShaderModule::InfoBegin() const
{
	return Current().ubo_info.cbegin();
}

hash_table<string, UboInfo>::const_iterator ShaderModule::InfoEnd() const
{
	return Current().ubo_info.cend();
}

hash_table<uint32_t, vk::UniqueDescriptorSetLayout>::const_iterator ShaderModule::LayoutsBegin() const
{
	return Current().layouts.cbegin();;
}

hash_table<uint32_t, vk::UniqueDescriptorSetLayout>::const_iterator ShaderModule::LayoutsEnd() const
{
	return Current().layouts.cend();
}

//UboInfo& ShaderModule::GetLayout(string uniform_name)
//{
//	return ubo_info[uniform_name];
//}

const UboInfo& ShaderModule::GetLayout(const string& uniform_name) const
{
	auto itr = Current().ubo_info.find(uniform_name);
	// TODO: insert return statement here
	
	return (itr!= Current().ubo_info.end())?itr->second:(*(UboInfo*)nullptr);
}
void DoNothing();
ShaderModule::~ShaderModule()
{
	DoNothing();
}

std::optional<uint32_t> ShaderModule::Data::GetBinding(uint32_t location) const
{
	std::optional<uint32_t> result{};
	auto itr = loc_to_bind.find(location);
	result = (itr != loc_to_bind.end()) ? itr->second : result;
	return result;
}

}