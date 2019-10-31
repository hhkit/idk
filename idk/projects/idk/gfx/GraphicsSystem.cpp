#include "stdafx.h"
#include "GraphicsSystem.h"
#include <core/GameObject.h>
#include <gfx/MeshRenderer.h>
#include <anim/Animator.h>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/RenderObject.h>
#include <particle/ParticleSystem.h>
#include <gfx/Font.h>

#include <gfx/DebugRenderer.h>
#include <gfx/Mesh.h>
//#include <gfx/CameraControls.h>

#include <atomic>

namespace idk
{
	enum class RequestState
	{
		eInvalid,
		eNeedRender,
		eRendered
	};

	struct RenderRequest
	{
		CameraData camera;
		vector<RenderObject> mesh_render;
		vector<AnimatedRenderObject> skinned_mesh_render;
		vector<SkeletonTransforms> skeleton_transforms;
		std::atomic<RequestState> _state = RequestState::eInvalid;

		void MarkRendered()
		{
			_state = RequestState::eRendered;
		}

		bool NeedsRendering()const
		{
			return _state == RequestState::eNeedRender;
		}

		bool Ready()const
		{
			return _state == RequestState::eRendered;
		}
		void Clear()
		{
			_state = RequestState::eInvalid;
		}
		void Set(RenderRequest&& req)
		{
			camera = std::move(req.camera);
			mesh_render = std::move(req.mesh_render);
			skinned_mesh_render = std::move(req.skinned_mesh_render);
			skeleton_transforms = std::move(req.skeleton_transforms);
			_state = RequestState::eNeedRender;
		}

	};
	void GraphicsSystem::PrepareLights(span<Light> lights)
	{
		for (auto& light : lights)
		{
			light.InitShadowMap();
		}
	}
	void GraphicsSystem::BufferedLightData(vector<LightData>& out)
	{
		out = object_buffer.at(curr_draw_buffer).lights;
	}
	void GraphicsSystem::RenderObjData(vector<RenderObject>& out)
	{
		out = object_buffer.at(curr_draw_buffer).mesh_render;
	}
	void GraphicsSystem::AnimatedRenderObjData(vector<AnimatedRenderObject>& out)
	{
		out = object_buffer.at(curr_draw_buffer).skinned_mesh_render;
	}

	void GraphicsSystem::PrepareSkeletonTransforms(const vector<Handle<Animator>>& animators, vector<SkeletonTransforms>& buffer_out, hash_table<Handle<Animator>, size_t>& index_table_out)
	{
		for (auto& animator : animators)
		{
			PrepareSkeletonTransform(*animator, buffer_out, index_table_out);
		}
	}

	void GraphicsSystem::PrepareSkeletonTransform(const Animator& animator, vector<SkeletonTransforms>& buffer_out, hash_table<Handle<Animator>, size_t>& index_table_out)
	{
		index_table_out.emplace(animator.GetHandle(), buffer_out.size());
		buffer_out.emplace_back(
			SkeletonTransforms{ std::move(animator.BoneTransforms()) } // generate this from the skeletons
			//SkeletonTransforms{ vector<mat4>{3} }
		);
	}

	std::optional<AnimatedRenderObject> GraphicsSystem::GetAnimatedRenderObj(const SkinnedMeshRenderer& skinned_mesh_renderer, const hash_table<Handle<Animator>, size_t>& index_table)
	{
		if (!skinned_mesh_renderer.IsActiveAndEnabled())
			return std::nullopt;
		AnimatedRenderObject ro = skinned_mesh_renderer.GenerateRenderObject();
		// @Joseph: GET PARENT IN THE FUTURE WHEN EACH MESH GO HAS ITS OWN SKINNED MESH RENDERER
		const auto parent = skinned_mesh_renderer.GetGameObject()->Parent();
		if (!parent)
			return std::nullopt;
		const auto animator = parent->GetComponent<Animator>();
		if (!animator)
			return std::nullopt;
		
		auto itr = index_table.find(animator);
		if (itr == index_table.end())
			return std::nullopt;

		ro.skeleton_index = itr->second;
		ro.config = mesh_render_config;
		return ro;
	}

	std::optional<RenderObject> GraphicsSystem::GetRenderObj(const MeshRenderer& skinned_mesh_renderer)
	{
		if (!skinned_mesh_renderer.IsActiveAndEnabled())
			return std::nullopt;
		auto obj =skinned_mesh_renderer.GenerateRenderObject();
		obj.config = mesh_render_config;
		return obj;		
	}

	size_t GraphicsSystem::AddRenderRequest(RenderRequest&& request)
	{

		CameraData& camera = request.camera;
		vector<RenderObject>& mesh_render = request.mesh_render;
			vector<AnimatedRenderObject>& skinned_mesh_render = request.skinned_mesh_render;
		vector<SkeletonTransforms> &skeleton_transforms = request.skeleton_transforms;
		//Todo: Add shaders
		return render_requests.emplace_back(SpecialRenderBuffer{ camera,std::move(mesh_render),std::move(skinned_mesh_render),std::move(skeleton_transforms),false });
	}

	bool GraphicsSystem::RenderRequestStatus(size_t index)
	{
		return render_requests[index].done_flag;
	}

	void GraphicsSystem::LateInit()
	{
		LoadShaders();
	}
	void GraphicsSystem::SortCameras()
	{
		Core::GetGameState().SortObjectsOfType<Camera>([](auto& lhs, auto& rhs) {return lhs.depth <= rhs.depth; });

	}
	void GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer> mesh_renderers,
		span<Animator> animators,
		span<SkinnedMeshRenderer> skinned_mesh_renderers,
        span<ParticleSystem> ps,
		span<Font> fonts,
		span<const class Transform>, 
		span<const Camera> cameras, 
		span<const Light> lights)
	{
		if (!mesh_render_config)
		{
			mesh_render_config = std::make_shared<pipeline_config>();
			mesh_render_config->fill_type = FillType::eFill;
			mesh_render_config->prim_top = PrimitiveTopology::eTriangleList;
		}
		// todo: scenegraph traversal
		RenderBuffer result{};
		result.camera.reserve(cameras.size());

		// memcpy the lights until there is a smarter implementation
		result.lights.reserve(lights.size());
		for (auto& elem : lights)
		{
			//result.light_camera_data.emplace_back(elem.GenerateCameraData());//Add the camera needed for the shadowmap
			result.lights.emplace_back(elem.GenerateLightData());
		}

		hash_table<Handle<Animator>, size_t> skeleton_indices;

		for (auto& elem : animators)
			PrepareSkeletonTransform(elem, result.skeleton_transforms, skeleton_indices);

		for (auto& elem : skinned_mesh_renderers)
		{
			auto ro = GetAnimatedRenderObj(elem, skeleton_indices);
			if (ro)
				result.skinned_mesh_render.emplace_back(std::move(*ro));
		}

		for (auto& camera : cameras)
		{
			if(camera.is_scene_camera)
				result.curr_scene_camera = camera.GenerateCameraData();
			result.camera.emplace_back(camera.GenerateCameraData());
		}

		for (auto& elem : mesh_renderers)
		{
			auto obj = GetRenderObj(elem);
			if (obj)
			{
				auto& render_obj = *obj;
				//Core::GetSystem<DebugRenderer>().Draw(render_obj.mesh->bounding_volume * render_obj.transform, color{ 0,0,1 });
				result.mesh_render.emplace_back(std::move(render_obj));
			}
		}

		result.renderer_vertex_shaders[VSkinnedMesh] = renderer_vertex_shaders[VSkinnedMesh];
		result.renderer_vertex_shaders[VNormalMesh] = renderer_vertex_shaders[VNormalMesh];


        for (auto& elem : ps)
        {
            if (elem.renderer.enabled && elem.data.num_alive)
            {
                const auto sz = elem.data.num_alive;
                auto& render_data = result.particle_render_data.emplace_back();

                render_data.particles.resize(sz);

                for (uint16_t i = 0; i < sz; ++i)
                    render_data.particles[i].position = elem.data.position[i] * elem.transform.scale;
                if (!elem.main.in_world_space)
                {
                    mat3 rot{ elem.transform.rotation };
                    for (auto& p : render_data.particles)
                        p.position = elem.transform.position + rot * p.position;
                }

                for (uint16_t i = 0; i < sz; ++i)
                    render_data.particles[i].rotation = elem.data.rotation[i];
                for (uint16_t i = 0; i < sz; ++i)
                    render_data.particles[i].size = elem.data.size[i];
                for (uint16_t i = 0; i < sz; ++i)
                    render_data.particles[i].color = elem.data.color[i];

                render_data.material_instance = elem.renderer.material;
            }
        }

		for (auto& f : fonts)
		{
			auto& render_data = result.font_render_data.emplace_back();
			//f.RenderText();
			render_data = f.fontData;
		}


		SubmitBuffers(std::move(result));
	}

	void GraphicsSystem::LoadShaders()
	{
		///////////////////////Load vertex shaders
		//renderer_vertex_shaders[VDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.vert",false);
		renderer_vertex_shaders[VNormalMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/mesh.vert",false);
		renderer_vertex_shaders[VSkinnedMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skinned_mesh.vert", false);
		renderer_vertex_shaders[VSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.vert", false);
		renderer_vertex_shaders[VPBRConvolute] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.vert", false);
		renderer_vertex_shaders[VFsq] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/fsq.vert", false);
		renderer_vertex_shaders[VFont] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/font.vert", false);

		////////////////////Load fragment Shaders
		//renderer_fragment_shaders[FDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FPBRConvolute] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.frag", false);
		renderer_fragment_shaders[FSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.frag", false);
		renderer_fragment_shaders[FBrdf] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/brdf.frag", false);
		renderer_fragment_shaders[FFont] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/font.frag", false);

		////////////////////Load geometry Shaders
		renderer_geometry_shaders[GSinglePassCube] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/single_pass_cube.geom", false);

		LoadShaderImpl();
	}

	void GraphicsSystem::SwapWritingBuffer()
	{
		//write_buffer_dirty = true;
		//tmp
		curr_draw_buffer= curr_write_buffer;
	}

	void GraphicsSystem::SubmitBuffers(RenderBuffer&& buffer)
	{
		object_buffer[curr_write_buffer] = std::move(buffer);
		SwapWritingBuffer();
	}

	//GraphicsSystem::TempLight::TempLight(const Light& l, GraphicsSystem& sys) :light{ l }
	//{
	//	auto init_map = [](auto& light) {
	//		return light.InitShadowMap();
	//	};
	//	auto get_id = [](auto& light) {
	//		return light.unique_id();
	//	};
	//	;
	//	auto id = std::visit(get_id, l.light);
	//	auto& pool = sys.shadow_map_pool[id];
	//	auto& next_index = sys.shadow_map_pool_index[id];
	//	if (pool.size() <= next_index)
	//	{
	//		pool.emplace_back(std::visit(init_map, l.light));
	//	}
	//	shadow_map = pool[next_index++];
	//}

	//CameraData GraphicsSystem::TempLight::GenerateCameraData()
	//{
	//	auto data =light.GenerateCameraData();
	//	data.render_target = shadow_map;
	//	return data;
	//}
	//
	//LightData GraphicsSystem::TempLight::GenerateLightData()
	//{
	//	auto data = light.GenerateLightData();
	//	data.light_map = shadow_map;
	//	return data;
	//}

}