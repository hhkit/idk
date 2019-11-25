#pragma once
#include <gfx/GraphicsSystem.h>
#include <gfx/Light.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/vector_buffer.h>
namespace idk::vkn
{
	struct buffer_info
	{
		vk::Buffer buffer{};
		uint32_t offset{};

		buffer_info() = default;
		buffer_info(vk::Buffer buf, uint32_t off) :buffer{ buf }, offset{ off }
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

	struct BufferUpdateInst
	{
		hlp::vector_buffer* buffer{};
		string_view data{};
		uint32_t offset{};
	};

	struct DbgDrawCall
	{
		//set 0 = vtx, set 1 = instance
		hash_table<uint32_t, buffer_info> mesh_buffer; //may be simplified
		buffer_info index_buffer;
		uint32_t num_indices   {};
		uint32_t num_instances {};
		uint32_t num_vertices  {};

		void RegisterBuffer(DbgBufferType type, uint32_t binding, buffer_info info);
		void SetNum(uint32_t num_inst, uint32_t num_vert);

		void Bind(vk::CommandBuffer cmd_buffer)const;
		void Draw(vk::CommandBuffer cmd_buffer)const;
	};

	using shadow_map_t = std::variant<RscHandle<Texture>, RscHandle<CubeMap>>;

	struct SharedGraphicsState
	{
		vector<BufferUpdateInst> update_instructions;
		RscHandle<Texture> BrdfLookupTable;
		const vector<LightData>* lights;
		hlp::vector_buffer inst_mesh_render_buffer;
		const vector<InstRenderObjects>* instanced_ros;
		const vector<ParticleObj>*        particle_data{};
		vector<ParticleRange>*            particle_range;
		hlp::vector_buffer                particle_buffer;
		//vector<shadow_map_t> shadow_maps;

		void Init(const vector<LightData>& light_data, const vector<InstRenderObjects>& iro);

		void Reset();

		const vector<LightData>& Lights()const;
		//vector<shadow_map_t>& ShadowMaps();
		//const vector<shadow_map_t>& ShadowMaps()const;
	};

	
	struct ProcessedMaterial
	{
		using data_block_t = string;
		using offset_t = size_t;
		using texture_table_t =hash_table<string, span<RscHandle<Texture>>>;
		using uniform_table_t =hash_table<string, string_view>;
		texture_table_t tex_table;
		uniform_table_t ubo_table;
		data_block_t data_block;
		vector<RscHandle<Texture>> texture_block;
		RscHandle<ShaderProgram> shader;

		ProcessedMaterial() = default;
		ProcessedMaterial(RscHandle<MaterialInstance> inst);
	};

	struct CoreGraphicsState
	{
		SharedGraphicsState* shared_gfx_state;
		vector<size_t> active_lights; //If we are somehow able to cull the lights that are completely not rendered.
		vector<const RenderObject*> mesh_render;
		vector<const AnimatedRenderObject*> skinned_mesh_render;
		hash_table<RscHandle<MaterialInstance>, ProcessedMaterial> material_instances;
		const vector<SkeletonTransforms>* skeleton_transforms;


		void ProcessMaterialInstances();
	};

	struct GraphicsState : CoreGraphicsState
	{
		CameraData camera; 
		GraphicsSystem::RenderRange range;
		const vector<LightData>* lights;
		vector<RscHandle<Texture>> shadow_maps_2d  ;
		vector<RscHandle<CubeMap>> shadow_maps_cube;

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

		void Init(const GraphicsSystem::RenderRange& data, const vector<LightData>& lights, const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms);
		void CullAndAdd(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects);
	};


	struct PreRenderData :CoreGraphicsState
	{
		const vector<CameraData>* cameras;
		const vector<InstancedData>* inst_mesh_buffer;
		const vector<GraphicsSystem::LightRenderRange>* shadow_ranges;
		//RscHandle<ShaderProgram> mesh_vtx;
		//RscHandle<ShaderProgram> skinned_mesh_vtx;
		array<RscHandle<ShaderProgram>, VertexShaders::VMax>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		void Init(const vector<RenderObject>& render_objects, const vector<AnimatedRenderObject>& skinned_render_objects, const vector<SkeletonTransforms>& s_transforms, const vector<InstancedData>& inst_mesh_render_buffer);
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