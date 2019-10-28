#pragma once
#include <gfx/GraphicsSystem.h>
#include <gfx/Light.h>
#include <vkn/VulkanPipeline.h>

namespace idk::vkn
{
	struct buffer_info
	{
		vk::Buffer buffer{};
		uint32_t offset{};

		buffer_info() = default;
		buffer_info(vk::Buffer buf,uint32_t off) :buffer{buf},offset{off}
		{
		}
		bool operator==(const buffer_info& bi)const { return buffer == bi.buffer && offset == bi.offset; }
	};

	enum DbgBufferType
	{
		ePerVtx=0,
		ePerInst,
		eSize
	};

	struct DbgDrawCall
	{
		//set 0 = vtx, set 1 = instance
		hash_table<uint32_t, buffer_info> mesh_buffer[s_cast<uint32_t>(DbgBufferType::eSize)]; //may be simplified
		buffer_info index_buffer;
		uint32_t num_indices   {};
		uint32_t num_instances {};
		uint32_t num_vertices  {};

		void RegisterBuffer(DbgBufferType type, uint32_t binding, buffer_info info)
		{
			mesh_buffer[s_cast<uint32_t>(type)][binding] = info;
		}
		void SetNum(uint32_t num_inst, uint32_t num_vert) { num_instances = num_inst; num_vert = num_vertices; }

		void Bind(vk::CommandBuffer cmd_buffer,const DbgDrawCall* prev)const
		{
			auto* pbuffers = &mesh_buffer[DbgBufferType::ePerVtx];
			if (!prev || prev->mesh_buffer[DbgBufferType::ePerVtx] != *pbuffers)
			{
				auto& buffers = *pbuffers;
				for (auto& [binding, buffer] : buffers)
				{
					cmd_buffer.bindVertexBuffers(binding,buffer.buffer,buffer.offset);
				}
			}
			auto& buffers = mesh_buffer[DbgBufferType::ePerInst];
			for (auto& [binding, buffer] : buffers)
			{
				cmd_buffer.bindVertexBuffers(binding, buffer.buffer, buffer.offset);
			}

		}
		void Draw(vk::CommandBuffer cmd_buffer)const
		{
			if (num_indices)
				cmd_buffer.drawIndexed(num_indices, num_instances, index_buffer.offset, 0, 0);
			else
				cmd_buffer.draw(num_vertices,num_instances, 0, 0);
		}
	};

	using shadow_map_t = std::variant<RscHandle<Texture>, RscHandle<CubeMap>>;

	struct SharedGraphicsState
	{
		RscHandle<Texture> BrdfLookupTable;
		const vector<LightData>* lights;
		//vector<shadow_map_t> shadow_maps;

		void Init(const vector<LightData>& light_data);

		const vector<LightData>& Lights()const;
		//vector<shadow_map_t>& ShadowMaps();
		//const vector<shadow_map_t>& ShadowMaps()const;
	};
	
	struct GraphicsState
	{
		SharedGraphicsState* shared_gfx_state =nullptr;
		CameraData camera;
		const vector<LightData>* lights;
		vector<size_t> active_lights;//indices to corresponding lights in shared_gfx_state
		vector<RscHandle<Texture>> shadow_maps_2d  ;
		vector<RscHandle<CubeMap>> shadow_maps_cube;

		vector<const RenderObject*> mesh_render;
		vector<const AnimatedRenderObject*> skinned_mesh_render;

		const vector<SkeletonTransforms>* skeleton_transforms;

		bool clear_render_target = false;

		// \param light_index
		// the index of the light in shared_gfx_state
		const LightData* ActiveLight(size_t light_index)const;
		//std::optional<RscHandle<Texture>> Shadow2D(size_t light_index)const;
		//std::optional<RscHandle<CubeMap>> ShadowCube(size_t light_index)const;
		//RscHandle<ShaderProgram> mesh_vtx;
		//RscHandle<ShaderProgram> skinned_mesh_vtx;
		array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;

		const vector<SkeletonTransforms>& GetSkeletonTransforms()const { return *skeleton_transforms; }

		vector<const DbgDrawCall*> dbg_render;
		const VulkanPipeline* dbg_pipeline;

		void Init(const CameraData& data, const vector<LightData>& lights, const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms);
		void CullAndAdd(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects);
	};


	struct PreRenderData
	{
		SharedGraphicsState* shared_gfx_state;
		const vector<CameraData>* cameras;
		vector<size_t> active_lights; //If we are somehow able to cull the lights that are completely not rendered.
		vector<const RenderObject*> mesh_render;
		vector<const AnimatedRenderObject*> skinned_mesh_render;

		const vector<SkeletonTransforms>* skeleton_transforms;

		//RscHandle<ShaderProgram> mesh_vtx;
		//RscHandle<ShaderProgram> skinned_mesh_vtx;
		array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		void Init(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms);
	};

	//Doesn't seem feasible, but might make for some kind of reference in the future
	//struct InstancedRO
	//{
	//	const RenderObject* base_object;
	//	//binding, data
	//	hash_table<uint32_t, string> instanced_data;
	//	size_t num_instances;
	//
	//	void SetBase(const RenderObject& base_obj) 
	//	{
	//		base_object = &base_obj;
	//	}
	//	bool CompareRO(const RenderObject& ro)
	//	{
	//		return
	//			ro.attrib_bindings == base_object->attrib_bindings
	//			//&&
	//			//ro.material_instance == base_object->material_instance
	//			&&
	//			ro.mesh == base_object->mesh;
	//	}
	//	//Ignores ROs which differ from the base in other ways than instanced_attribs
	//	void AddInstance(const RenderObject& ro, bool filter_diff=false) 
	//	{
	//		if (!filter_diff || CompareRO(ro))
	//		{
	//			
	//		}
	//	}
	//};
}