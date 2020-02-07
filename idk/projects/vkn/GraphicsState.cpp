#include "pch.h"
#include "GraphicsState.h"
#include <gfx/RenderTarget.h>
#include <gfx/Framebuffer.h>
#include <ds/span.inl>
#include <res/ResourceHandle.inl>
namespace idk::vkn
{
	const LightData* GraphicsState::ActiveLight(size_t light_index) const
	{
		return &shared_gfx_state->Lights()[light_index];
	}
	void GraphicsState::Init(const GraphicsSystem::RenderRange& data, const vector<size_t>& all_active_lights, const vector<size_t>& active_directional_light, const vector<LightData>& lights_data, const std::map<Handle<GameObject>, CamLightData>& d_lm, const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms)
	{
		camera = data.camera;
		range = data;
		lights = &lights_data;
		d_lightmaps = &d_lm;
		mesh_render.clear();
		skinned_mesh_render.clear();
		active_lights.clear();
		active_dir_lights.clear();
		{
			size_t i = 0, j =0;
			RscHandle<Texture> def_2d;
			RscHandle<CubeMap> def_cube;

			active_lights.insert(active_lights.end(), all_active_lights.begin() + data.light_begin, all_active_lights.begin() + data.light_end);
			shadow_maps_2d.resize(active_lights.size(), def_2d);
			active_dir_lights.insert(active_dir_lights.end(), active_directional_light.begin() + data.dir_light_begin, active_directional_light.begin() + data.dir_light_end);
			//shadow_maps_directional.resize(active_directional_light.size(), def_2d);
			shadow_maps_cube.resize(active_lights.size(), def_cube);
			for (auto& light_idx : active_lights)
			{
				auto& light = lights_data[light_idx];
				if (light.index == 2)//spotlight
				{
					for(auto& elem: light.light_maps)
						shadow_maps_2d[i]=(s_cast<RscHandle<Texture>>(elem.light_map->DepthAttachment()));
					//shadow_maps_cube[i]=(def_cube);
				}
				//else if(light.index == 1) //directional light
				//{
				//	if(!light.light_maps.empty())
				//		shadow_maps_2d[i] = (s_cast<RscHandle<Texture>>(light.light_maps[0].light_map->DepthAttachment()));
				//	for (auto& elem : light.light_maps)
				//	{
				//		shadow_maps_directional.at(j++) = (s_cast<RscHandle<Texture>>(elem.light_map->DepthAttachment()));
				//	}
				//}

				++i;
				
			}
			//for (auto& dir_light_idx : active_directional_light)
			//{
			//	auto& light = lights_data[dir_light_idx];
			//	//if (!light.light_maps.empty())
			//		//shadow_maps_2d[i] = (s_cast<RscHandle<Texture>>(light.light_maps[0].light_map->DepthAttachment()));
			//	for (auto& elem : light.light_maps)
			//		shadow_maps_directional[j++] = (s_cast<RscHandle<Texture>>(elem.light_map->DepthAttachment()));
			//	//++j;
			//}
		}
		skeleton_transforms = &s_transforms;
		CullAndAdd(render_objects, skinned_render_objects);
	}

	void CoreGraphicsState::ProcessMaterialInstances()
	{
		auto AddMatInst = [](auto& material_instances, const RenderObject* p_ro)
		{
			auto mat_inst = p_ro->material_instance;
			if (material_instances.find(mat_inst) == material_instances.end())
			{
				material_instances.emplace(mat_inst, ProcessedMaterial{ mat_inst });
			}
		};
		for (auto& p_ro : mesh_render)
		{
			AddMatInst(material_instances, p_ro);
		}
		for (auto& p_ro : skinned_mesh_render)
		{
			AddMatInst(material_instances, p_ro);
		}
		for (auto& p_ro : *shared_gfx_state->instanced_ros)
		{
			AddMatInst(material_instances, &p_ro);
		}
		if(shared_gfx_state->particle_range)
		for (auto& part_range : *shared_gfx_state->particle_range)
		{
			material_instances[part_range.material_instance] = ProcessedMaterial{ part_range.material_instance };
		}
	}

void GraphicsState::CullAndAdd(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects)
{
	mesh_render.reserve(render_objects.size() + mesh_render.size());
	for (auto& ro : render_objects)
	{
		mesh_render.emplace_back(&ro);
	}
	skinned_mesh_render.reserve(skinned_render_objects.size() + skinned_mesh_render.size());
	for (auto& ro : skinned_render_objects)
	{
		skinned_mesh_render.emplace_back(&ro);
	}
}

void SharedGraphicsState::Init(const vector<LightData>& light_data, const vector<InstRenderObjects>& iro)
{
	lights = &light_data;
	instanced_ros = &iro;
	//shadow_maps.resize(light_data.size());
}

void SharedGraphicsState::Reset()
{
	lights=nullptr;
	instanced_ros = nullptr;
	update_instructions.resize(0);
}

const vector<LightData>& SharedGraphicsState::Lights() const
{ 
	return *lights; 
}
/*
vector<shadow_map_t>& SharedGraphicsState::ShadowMaps()
{
	return shadow_maps;
}

const vector<shadow_map_t>& SharedGraphicsState::ShadowMaps() const
{
	return shadow_maps;
}
*/
void PreRenderData::Init(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms, const vector<InstancedData>& inst_mesh_render_buffer)
{
	mesh_render.clear();
	skinned_mesh_render.clear();

	mesh_render.reserve(render_objects.size());
	for (auto& ro : render_objects)
	{
		mesh_render.emplace_back(&ro);
	}
	skinned_mesh_render.reserve(skinned_render_objects.size() );
	for (auto& ro : skinned_render_objects)
	{
		skinned_mesh_render.emplace_back(&ro);
	}
	skeleton_transforms = &s_transforms;

	inst_mesh_buffer = &inst_mesh_render_buffer;
}

ProcessedMaterial::ProcessedMaterial(RscHandle<MaterialInstance> inst)
{
	{
		auto& mat_inst = *inst;
		[[maybe_unused]] auto& mat = *mat_inst.material;
		auto mat_cache = mat_inst.get_cache();
		using offset_t =size_t;
		data_block.reserve(std::max(data_block.size(), 32ui64));
		hash_table<string, std::pair<offset_t, size_t>> ubo_stored;
		ubo_stored.reserve(mat_cache.uniforms.size());
		hash_table<string, std::pair<offset_t, size_t>> tex_stored;
		tex_stored.reserve(mat_cache.uniforms.size());
		for (auto itr = mat_cache.uniforms.begin(); itr != mat_cache.uniforms.end(); ++itr)
		{
			if (mat_cache.IsUniformBlock(itr))
			{
				auto block = mat_cache.GetUniformBlock(itr);
				offset_t offset = data_block.size();
				size_t size = block.size();
				//Add to block
				data_block.append(block.data(), size);
				//store offset and stuff ( don't store ptrs due to invalidation)
				ubo_stored[itr->first]={ offset,size };
			}
			else if (mat_cache.IsImageBlock(itr))
			{
				auto block = mat_cache.GetImageBlock(itr);
				offset_t offset = texture_block.size();
				size_t size = block.size();
				//Add to block
				texture_block.insert(texture_block.end(),block.begin(),block.end());
				auto name = itr->first.substr(0, itr->first.find_first_of('['));
				//store offset and stuff ( don't store ptrs due to invalidation)
				tex_stored[name] = { offset,size };
			}
		}
		for (auto& [name, block_range] : ubo_stored)
		{
			auto& [offset, size] = block_range;
			auto ptr = data_block.data();
			ubo_table[name] = string_view(ptr + offset, size);
		}
		for (auto& [name, block_range] : tex_stored)
		{
			auto& [offset, size] = block_range;
			auto ptr = texture_block.data();
			tex_table[name] = span<RscHandle<Texture>>{ ptr + offset,ptr + size };
		}
		shader = mat._shader_program;
	}
}

void DbgDrawCall::RegisterBuffer([[maybe_unused]]DbgBufferType type, uint32_t binding, buffer_info info)
{
	mesh_buffer[binding] = info;
}

void DbgDrawCall::SetNum(uint32_t num_inst, uint32_t num_vert) { num_instances = num_inst; num_vert = num_vertices; }

void DbgDrawCall::Bind(vk::CommandBuffer cmd_buffer) const
{
	/*auto& buffers = mesh_buffer.find(DbgBufferType::ePerVtx)->second;
	if (!prev || !(prev->mesh_buffer.find(DbgBufferType::ePerVtx)->second == buffers))
	{
	for (auto& [binding, buffer] : buffers)
	{
	cmd_buffer.bindVertexBuffers(binding,buffer.buffer,buffer.offset);
	}
	}*/
	auto& buffers = mesh_buffer;
	for (auto& [binding, buffer] : buffers)
	{
		cmd_buffer.bindVertexBuffers(binding, buffer.buffer, buffer.offset);
	}
}

void DbgDrawCall::Draw(vk::CommandBuffer cmd_buffer) const
{

	if (num_indices)
	{
		cmd_buffer.bindIndexBuffer(index_buffer.buffer, index_buffer.offset, vk::IndexType::eUint16);
		cmd_buffer.drawIndexed(num_indices, num_instances, index_buffer.offset, 0, 0);
	}
	else
		cmd_buffer.draw(num_vertices, num_instances, 0, 0);
}

}