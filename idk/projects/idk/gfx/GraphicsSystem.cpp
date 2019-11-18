#include "stdafx.h"
#include "GraphicsSystem.h"
#include <core/GameObject.h>
#include <gfx/MeshRenderer.h>
#include <anim/Animator.h>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/RenderObject.h>
#include <particle/ParticleSystem.h>
#include <gfx/Font.h>
#include <ui/Image.h>
#include <ui/RectTransform.h>
#include <ui/Canvas.h>
#include <common/Transform.h>
#include <app/Application.h>

#include <gfx/DebugRenderer.h>
#include <gfx/Mesh.h>
//#include <gfx/CameraControls.h>

#include <atomic>

#include <meta/comparator.inl>

#include <math/shapes/frustum.h>

struct guid_64
{
	uint64_t mem1;
	uint64_t mem2;
};
using guid_comparator = idk::ordered_comparator<guid_64, idk::PtrMem<decltype(&guid_64::mem1), & guid_64::mem1>, idk::PtrMem<decltype(&guid_64::mem2), & guid_64::mem2>>;

union guid_u {
	idk::Guid guid;
	guid_64 data;
};
static inline bool operator<(const idk::Guid& lhs, const idk::Guid& rhs) noexcept
{
	guid_u lmap{ lhs },rmap{ rhs };

	return guid_comparator{}(lmap.data,rmap.data);
}
template<typename Rsc>
static inline bool operator<(const idk::RscHandle<Rsc>& lhs, const idk::RscHandle<Rsc>& rhs) noexcept
{
	return lhs.guid < rhs.guid;
}

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

		ro.skeleton_index = static_cast<unsigned>(itr->second);
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

	bool GraphicsSystem::is_deferred() const
	{
		return _is_deferred;
	}

	bool GraphicsSystem::is_deferred(bool enable)
	{
		return _is_deferred = enable;
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
		Core::GetGameState().SortObjectsOfType<Camera>([](auto& lhs, auto& rhs) {return lhs.depth < rhs.depth; });

	}

	
	GenericInstancedRenderObjects CreateIROInfo(const RenderObject& ro)
	{
		return GenericInstancedRenderObjects{
			ro.obj_id,
			ro.mesh,
			ro.material_instance,
			{},
			{},
			ro.cast_shadows,
			ro.receive_shadows,
			ro.renderer_req,
			ro.config,
		};
	}
	//returns indices to the start and one past the end
	std::pair<size_t,size_t> CullAndBatchRenderObjects(const CameraData& camera,const vector<RenderObject>& ro, vector<InstRenderObjects>& inst, vector<InstancedData>& instanced_data)
	{

		const auto frust = camera_vp_to_frustum(camera.projection_matrix * camera.view_matrix);
		//Keep track of the batches culled by this frustum
		std::pair<size_t, size_t> result{ inst.size() ,inst.size() };
		std::optional<decltype(ro.begin())> oprev{};
		InstRenderObjects* inst_itr{};
		for (auto itr = ro.begin(); itr < ro.end(); ++itr)
		{
			const auto bv = itr->mesh->bounding_volume* itr->transform;
			if (frust.contains(bv))
			{
				if (!oprev || ![](auto& itr, auto& prev) {
					return itr->mesh == prev->mesh && itr->material_instance == prev->material_instance;
					}(itr, *oprev))
				{

					inst_itr = &inst.emplace_back(CreateIROInfo(*itr));
					inst_itr->instanced_index = instanced_data.size();
					oprev = itr;
				}
				//Keep track of the number of instances to be render for this frustum
					auto tfm = camera.view_matrix * itr->transform;
				instanced_data.emplace_back(InstancedData{ tfm,tfm.inverse().transpose() });
				inst_itr->num_instances++;
			}
		}
		result.second = inst.size();
		return result;
	}
	/* disabled until we're ready to instance animated render objects too.
	std::pair<size_t, size_t>  CullAndBatchAnimatedRenderObjects(const frustum& frust, const vector<AnimatedRenderObject>& ro, vector<InstAnimatedRenderObjects>& inst)
	{
		std::pair<size_t, size_t> result{ inst.size() ,inst.size() };
		std::optional<decltype(ro.begin())> oprev{};
		InstAnimatedRenderObjects* inst_itr{};
		for (auto itr = ro.begin(); itr < ro.end(); ++itr)
		{
			auto bv = itr->mesh->bounding_volume * itr->transform;
			if (frust.contains(bv))
			{
				if (!oprev || ![](auto& itr, auto& prev) {
					return itr->mesh == prev->mesh && itr->material_instance == prev->material_instance;
					}(itr, *oprev))
				{
					inst_itr = &inst.emplace_back(CreateIROInfo<InstAnimatedRenderObjects>(*itr));
					oprev = itr;
				}
					inst_itr->instanced_data.emplace_back(AnimatedInstancedData{ itr->velocity,itr->transform,itr->skeleton_index });
			}
		}
		result.second = inst.size();
		return result;
	}
	*/
	template<typename Vec>
	void ClearSwap(Vec& dst, Vec& src) noexcept
	{
		src.clear();
		std::swap(dst, src);
	}


	void GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer> mesh_renderers,
		span<Animator> animators,
		span<SkinnedMeshRenderer> skinned_mesh_renderers,
        span<ParticleSystem> ps,
		span<Font> fonts,
        span<Image> images,
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

		Core::GetGameState().SortObjectsOfType<MeshRenderer>(
			[](const MeshRenderer& lhs, const MeshRenderer& rhs)
			{
				const auto l_mat_inst = lhs.material_instance;
				const auto r_mat_inst = rhs.material_instance;
				const auto l_mat = l_mat_inst->material;
				const auto r_mat = r_mat_inst->material;
				const auto l_mesh = lhs.mesh;
				const auto r_mesh = rhs.mesh;

				constexpr static auto guid_hasher = std::hash<Guid>{};


				const auto l_mat_inst_hash = guid_hasher(lhs.material_instance.guid);
				const auto r_mat_inst_hash = guid_hasher(rhs.material_instance.guid);
				const auto l_mat_hash      = guid_hasher(l_mat_inst->material.guid);
				const auto r_mat_hash      = guid_hasher(r_mat_inst->material.guid);
				const auto l_mesh_hash     = guid_hasher(lhs.mesh.guid);
				const auto r_mesh_hash     = guid_hasher(rhs.mesh.guid);

				if (l_mat_hash < r_mat_hash)
					return true;

				if (l_mat_hash > r_mat_hash)
					return false;

				// hence the mats are the same

				if (l_mat_inst_hash < r_mat_inst_hash)
					return true;

				if (l_mat_inst_hash > r_mat_inst_hash)
					return false;

				// hence the instances are the same

				if (l_mesh_hash < r_mesh_hash)
					return true;

				return false;
			}
		);


		using ro_inst_comp = ordered_comparator<RenderObject,
			PtrMem<decltype(&RenderObject::material_instance), & RenderObject::material_instance>,
			PtrMem<decltype(&RenderObject::mesh), & RenderObject::mesh>
		>;
		using aro_inst_comp = ordered_comparator<AnimatedRenderObject,
			PtrMem<decltype(&RenderObject::material_instance), & RenderObject::material_instance>,
			PtrMem<decltype(&RenderObject::mesh), & RenderObject::mesh>,
			PtrMem<decltype(&AnimatedRenderObject::skeleton_index), &AnimatedRenderObject::skeleton_index>
		>;

		auto reset_render_buffer = [](RenderBuffer& rb)
		{
			RenderBuffer tmp{};
			std::swap(tmp, rb); //reinitialize the stuff that don't need to be swapped.
			ClearSwap(rb.camera                         ,tmp.camera                         );//clear then swap the stuff back into rb
			ClearSwap(rb.font_render_data               ,tmp.font_render_data               );//clear then swap the stuff back into rb
			ClearSwap(rb.instanced_mesh_render          ,tmp.instanced_mesh_render          );//clear then swap the stuff back into rb
			//ClearSwap(rb.instanced_skinned_mesh_render  ,tmp.instanced_skinned_mesh_render  );//clear then swap the stuff back into rb
			ClearSwap(rb.inst_mesh_render_buffer        ,tmp.inst_mesh_render_buffer        );//clear then swap the stuff back into rb
			//ClearSwap(rb.inst_skinned_mesh_render_buffer,tmp.inst_skinned_mesh_render_buffer);//clear then swap the stuff back into rb
			ClearSwap(rb.lights                         ,tmp.lights                         );//clear then swap the stuff back into rb
			ClearSwap(rb.light_camera_data              ,tmp.light_camera_data              );//clear then swap the stuff back into rb
			ClearSwap(rb.mesh_render                    ,tmp.mesh_render                    );//clear then swap the stuff back into rb
			ClearSwap(rb.skinned_mesh_render            ,tmp.skinned_mesh_render            );//clear then swap the stuff back into rb
			ClearSwap(rb.particle_render_data           ,tmp.particle_render_data           );//clear then swap the stuff back into rb
			ClearSwap(rb.skeleton_transforms            ,tmp.skeleton_transforms            );//clear then swap the stuff back into rb
		};

		// todo: scenegraph traversal
		RenderBuffer& result=GetWriteBuffer();
		result = RenderBuffer{};
		//reset_render_buffer(result); //change to this method to reduce reallocation count.
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
			if (elem.GetHandle().scene == Scene::prefab)
				continue;
			auto ro = GetAnimatedRenderObj(elem, skeleton_indices);
			if (ro)
				result.skinned_mesh_render.emplace_back(std::move(*ro));
		}

		for (auto& camera : cameras)
		{
			if (camera.GetHandle().scene == Scene::prefab)
				continue;

            if (camera.GetHandle().scene == Scene::editor)
            {
                result.curr_scene_camera_index = result.camera.size();
                result.camera.emplace_back(camera.GenerateCameraData());
            }
			else if (camera.enabled)
				result.camera.emplace_back(camera.GenerateCameraData());
		}

		for (auto& elem : mesh_renderers)
		{
			if (elem.GetHandle().scene == Scene::prefab)
				continue;

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


		std::sort(result.mesh_render.begin(), result.mesh_render.end(), ro_inst_comp{});
		std::sort(result.skinned_mesh_render.begin(), result.skinned_mesh_render.end(), aro_inst_comp{});

		for (auto& camera : result.camera)
		{
			RenderRange range{ camera };
			{
				const auto [start_index, end_index] = CullAndBatchRenderObjects(camera, result.mesh_render, result.instanced_mesh_render,result.inst_mesh_render_buffer);
				range.inst_mesh_render_begin = start_index;
				range.inst_mesh_render_end = end_index;
			}
			result.culled_render_range.emplace_back(range);
			//{
			//	auto [start_index, end_index] = CullAndBatchAnimatedRenderObjects(frustum, result.skinned_mesh_render, result.instanced_skinned_mesh_render);
			//	range.inst_mesh_render_begin = start_index;
			//	range.inst_mesh_render_end = end_index;
			//}
		}



		for (auto& f : fonts)
		{
			auto& render_data = result.font_render_data.emplace_back();
			//if (!f.textureAtlas)
				//f.textureAtlas = FontAtlas::defaults[FontDefault::SourceSansPro];
			f.RenderText();
			render_data = f.fontData;
		}

        for (auto& im : images)
        {
            const auto& go = *im.GetGameObject();
            const auto& rt = *go.GetComponent<RectTransform>();
            
            auto& render_data = result.ui_render_per_cam[rt.FindCanvas()->target_camera.index].emplace_back();
            const auto rect = rt.RectInCanvas();
            rect.size / vec2(Core::GetSystem<Application>().GetScreenSize());

            render_data.transform = go.Transform()->GlobalMatrix();
            render_data.material = im.material;
            render_data.data = ImageData{ im.texture };
        }


		//SubmitBuffers(std::move(result));
		SwapWritingBuffer();
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
		renderer_vertex_shaders[VUi] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/ui.vert", false);

		////////////////////Load fragment Shaders
		//renderer_fragment_shaders[FDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FPBRConvolute] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.frag", false);
		renderer_fragment_shaders[FSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.frag", false);
		renderer_fragment_shaders[FBrdf] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/brdf.frag", false);
		renderer_fragment_shaders[FFont] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/font.frag", false);
		renderer_fragment_shaders[FDeferredPost] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_post.frag", false);

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

	GraphicsSystem::RenderBuffer& GraphicsSystem::GetWriteBuffer()
	{
		// TODO: insert return statement here
		return object_buffer[curr_write_buffer];
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