#include "stdafx.h"
#include "GraphicsSystem.h"
#include "scene/SceneManager.h"
#include <core/GameObject.inl>
#include <gfx/MeshRenderer.h>
#include <anim/Animator.h>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/RenderObject.h>
#include <particle/ParticleSystem.h>
#include <gfx/TextMesh.h>
#include <ui/Image.h>
#include <ui/Text.h>
#include <ui/RectTransform.h>
#include <ui/Canvas.h>
#include <common/Transform.h>
#include <app/Application.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

#include <gfx/DebugRenderer.h>
#include <gfx/Mesh.h>
//#include <gfx/CameraControls.h>

#include <atomic>

#include <meta/comparator.inl>

#include <math/shapes/frustum.h>
#include <math/matrix_transforms.inl>

#include <parallel/ThreadPool.h>

#include <meta/meta.inl>
#include <ds/span.inl>
#include <ds/result.inl>

#include <variant>

#include <gfx/RenderTarget.h>


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

	ColorPickResult GraphicsSystem::ColorPick(vec2 picking_pt, CameraData camera)
	{
		auto sz = camera.render_target->Size();
		if (picking_pt.x > sz.x || picking_pt.x<0
			||
			picking_pt.y>sz.y || picking_pt.y < 0)
		{
			
			return ColorPickResult{ ColorPickResult::result_t{} };
		}
		auto& req = request_stack.emplace_front();
		
		return req.promise(picking_pt,camera);
	}

	void GraphicsSystem::BufferRequests()
	{
		std::move(request_stack.begin(), request_stack.end(), std::back_inserter(request_buffer));
		request_stack.clear();
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
	std::pair<size_t,size_t> CullAndBatchRenderObjects(const CameraData& camera,const vector<RenderObject>& ro,const vector<sphere>& bounding_vols, vector<InstRenderObjects>& inst, vector<InstancedData>& instanced_data,vector<GenericHandle>* handle_buffer=nullptr)
	{

		const auto frust = camera_vp_to_frustum(camera.projection_matrix * camera.view_matrix);
		//Keep track of the batches culled by this frustum
		std::pair<size_t, size_t> result{ inst.size() ,inst.size() };
		std::optional<decltype(ro.begin())> oprev{};
		InstRenderObjects* inst_itr{};
		auto bv_itr = bounding_vols.begin();
		for (auto itr = ro.begin(); itr < ro.end(); ++itr,++bv_itr)
		{
			const auto bv = *bv_itr;
			if ((itr->layer_mask&camera.culling_flags) &&frust.contains(bv))
			{
				if (!oprev || ![](auto& itr, auto& prev) {
					return (itr->mesh == prev->mesh) & (itr->material_instance == prev->material_instance);
					}(itr, *oprev))
				{
					inst_itr = &inst.emplace_back(CreateIROInfo(*itr));
					inst_itr->instanced_index = instanced_data.size();
					oprev = itr;
				}
				if (handle_buffer)
					handle_buffer->emplace_back(itr->obj_id);
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

	void ProcessParticles(
		const vector<ParticleRenderData>& unique_particles,
		vector<ParticleObj>& particle_buffer,
		vector<ParticleRange>& particle_render_data,
		GraphicsSystem::RenderRange& range
	)
	{
		auto& cam = range.camera;
		const vec3 cam_forward{ -cam.view_matrix[0][2], -cam.view_matrix[1][2], -cam.view_matrix[2][2] };
		const vec3 cam_pos = cam.view_matrix[3];
		range.inst_particle_begin = particle_render_data.size();
		for (auto& elem : unique_particles)
		{
			ParticleRange p_range{ elem.material_instance,particle_buffer.size(),static_cast<size_t>(std::distance(elem.particles.begin(), elem.particles.end())) };

			particle_buffer.insert(particle_buffer.end(), elem.particles.begin(),elem.particles.end());
				
			std::sort(particle_buffer.begin()+p_range.elem_offset, particle_buffer.begin() + p_range.elem_offset+p_range.num_elems,
				[cam_forward, cam_pos](const ParticleObj& a, const ParticleObj& b) {
					return (a.position - cam_pos).dot(cam_forward) > (b.position - cam_pos).dot(cam_forward); });
				
			particle_render_data.emplace_back(p_range);
		}
		range.inst_particle_end = particle_render_data.size();
	}

	void ProcessFonts(
		const vector<FontRenderData>& unique_fonts,
		vector<FontPoint>& font_buffer,
		vector<FontRange>& font_render_data,
		GraphicsSystem::RenderRange& range
	)
	{
		range.inst_font_begin = font_render_data.size();
		for (auto& elem : unique_fonts)
		{
			FontRange f_range{ font_buffer.size(),(size_t)std::distance(elem.coords.begin(), elem.coords.end()) };
			font_buffer.insert(font_buffer.end(), elem.coords.begin(), elem.coords.end());

		    font_render_data.emplace_back(f_range);
		}
		
		range.inst_font_end = font_render_data.size();
	}

	void ProcessCanvas(
		const vector<FontPoint>& unique_canvas_font,
		vector<UIAttriBlock>& font_buffer,
		//vector<FontPoint>& font_buffer,
		vector<UITextRange>& font_render_data,
		size_t& total_num_of_text
	)
	{
		//range.inst_font_begin = font_render_data.size();
		
		if (unique_canvas_font.size())
		{
			vector<vec2> posList;
			vector<vec2> uvList;
			size_t count = 0;
			for (auto& elem : unique_canvas_font)
			{
				auto res = elem.ConvertToPairs();
				posList.insert(posList.end(), res.first);
				uvList.insert(uvList.end(), res.second);
				++count;
			}
			auto start = posList.size();
			font_buffer.insert(font_buffer.end(), UIAttriBlock{ posList,uvList });
			++total_num_of_text;

			UITextRange f_range{ start,count };
			font_render_data.emplace_back(f_range);

		}
		//range.inst_font_end = font_render_data.size();
	}

	sphere bounding_box_to_loose_sphere(const aabb& box)
	{
		auto e = box.extents();
		float d = 0;
		for (auto axis_aligned_len : e)
		{
			d = std::max(0.0f, axis_aligned_len);
		}
		sphere result{ box.center(),d / 2 };
		return result;
	}

	void CullLights(const CameraData& camera,ShadowMapPool& sm_pool,vector<LightData>& lights,vector<LightData>& new_lights, vector<size_t>& active_light_buffer, vector<size_t>& directional_light_buffer, GraphicsSystem::RenderRange& range)
	{

		auto frustum = camera_vp_to_frustum(camera.projection_matrix * camera.view_matrix);
		range.light_begin = active_light_buffer.size();
		range.dir_light_begin = directional_light_buffer.size();
		
		//Perform camera light loop to populate the data
		float n_plane = camera.near_plane, f_plane = camera.far_plane;

		float diff = f_plane - n_plane;
		float first_end = n_plane + 0.2f * diff;
		float second_end = n_plane + 0.45f * diff;

		float cascadeiter[4] = { n_plane,first_end,second_end,f_plane };
		//float cascadeiter[3] = { n_plane,first_end,second_end };

		for (size_t i = 0; i < lights.size(); ++i)
		{
			auto& light = lights[i];
			switch (light.index)
			{
			case index_in_variant_v<PointLight, LightVariant>:
				{
					sphere sphere{ invert_rotation(light.v)[3],light.falloff };
					if(frustum.contains(sphere))
						active_light_buffer.emplace_back(i);

					light.camDataRef = camera;
				}
				break;
			case index_in_variant_v<SpotLight, LightVariant>:
			{
				auto bounding_box = camera_vp_to_bounding_box(light.vp);
				auto sphere = bounding_box_to_loose_sphere(bounding_box);
				if (frustum.contains(sphere))
				{
					active_light_buffer.emplace_back(i);

					light.camDataRef = camera;
				}
			}
				break;
			case index_in_variant_v<DirectionalLight, LightVariant>:
			{
				///////////////////////////////////////>>>>>>>>>>>>>>>>>>>>>>>
				unsigned k = 0, j = 1;
				light.camDataRef = camera;

				auto copy_light = light;

				copy_light.light_maps = sm_pool.GetShadowMaps(copy_light.index,copy_light.light_maps);
				
				for (auto& elem : copy_light.light_maps)
				{
					elem.SetCascade(camera, copy_light, cascadeiter[k], cascadeiter[j]);
					++k; ++j;
				}
				auto new_index = new_lights.size();
				new_lights.emplace_back(copy_light);
				active_light_buffer.emplace_back(lights.size() + new_index); //new_lights is gonna be appended at the back of the set
				directional_light_buffer.emplace_back(lights.size() + new_index);

				//active_light_buffer.emplace_back(i);

				
			}
				break;
			}

		}
		range.light_end = active_light_buffer.size();
		range.dir_light_end = directional_light_buffer.size();
	}
//#pragma optimize("",off)
	void GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer> mesh_renderers,
		span<Animator> animators,
		span<SkinnedMeshRenderer> skinned_mesh_renderers,
		span<ParticleSystem> ps,
		span<TextMesh> fonts,
		span<Text> texts,
		span<Image> images,
		span<const class Transform>,
		span<const Camera> cameras,
		span<const Light> lights)
	{
		BufferRequests();
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
				const auto l_mat_hash = guid_hasher(l_mat_inst->material.guid);
				const auto r_mat_hash = guid_hasher(r_mat_inst->material.guid);
				const auto l_mesh_hash = guid_hasher(lhs.mesh.guid);
				const auto r_mesh_hash = guid_hasher(rhs.mesh.guid);

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
			PtrMem<decltype(&RenderObject::material_instance), &RenderObject::material_instance>,
			PtrMem<decltype(&RenderObject::mesh), &RenderObject::mesh>
		>;
		using aro_inst_comp = ordered_comparator<AnimatedRenderObject,
			PtrMem<decltype(&RenderObject::material_instance), &RenderObject::material_instance>,
			PtrMem<decltype(&RenderObject::mesh), &RenderObject::mesh>,
			PtrMem<decltype(&AnimatedRenderObject::skeleton_index), &AnimatedRenderObject::skeleton_index>
		>;

		auto reset_render_buffer = [](RenderBuffer& rb)
		{
			RenderBuffer tmp{};
			std::swap(tmp, rb); //reinitialize the stuff that don't need to be swapped.
			ClearSwap(rb.camera, tmp.camera);//clear then swap the stuff back into rb
			ClearSwap(rb.lights, tmp.lights);//clear then swap the stuff back into rb
			ClearSwap(rb.active_light_buffer, tmp.active_light_buffer);
			ClearSwap(rb.directional_light_buffer, tmp.directional_light_buffer);
			ClearSwap(rb.d_lightmaps,tmp.d_lightmaps);
			//ClearSwap(rb.light_camera_data, tmp.light_camera_data);
			ClearSwap(rb.mesh_render, tmp.mesh_render);//clear then swap the stuff back into rb
			ClearSwap(rb.skinned_mesh_render, tmp.skinned_mesh_render);//clear then swap the stuff back into rb
			ClearSwap(rb.skeleton_transforms, tmp.skeleton_transforms);//clear then swap the stuff back into rb
			ClearSwap(rb.particle_render_data, tmp.particle_render_data);//clear then swap the stuff back into rb
			ClearSwap(rb.particle_buffer, tmp.particle_buffer);
			ClearSwap(rb.particle_range, tmp.particle_range);
			ClearSwap(rb.font_render_data, tmp.font_render_data);//clear then swap the stuff back into rb
			
			ClearSwap(rb.font_range, tmp.font_range);
			ClearSwap(rb.font_buffer, tmp.font_buffer);
			
			ClearSwap(rb.ui_render_per_canvas, tmp.ui_render_per_canvas);
			ClearSwap(rb.ui_canvas, tmp.ui_canvas);
			ClearSwap(rb.ui_text_buffer, tmp.ui_text_buffer);
			ClearSwap(rb.ui_text_range, tmp.ui_text_range);
			//ClearSwap(rb.ui_canvas_range, tmp.ui_canvas_range);
			ClearSwap(rb.instanced_mesh_render, tmp.instanced_mesh_render);//clear then swap the stuff back into rb
			//ClearSwap(rb.instanced_skinned_mesh_render  ,tmp.instanced_skinned_mesh_render  );//clear then swap the stuff back into rb
			ClearSwap(rb.inst_mesh_render_buffer, tmp.inst_mesh_render_buffer);//clear then swap the stuff back into rb
			//ClearSwap(rb.inst_skinned_mesh_render_buffer,tmp.inst_skinned_mesh_render_buffer);//clear then swap the stuff back into rb
			//ClearSwap(rb.light_camera_data, tmp.light_camera_data);//clear then swap the stuff back into rb
			ClearSwap(rb.culled_render_range, tmp.culled_render_range);//clear then swap the stuff back into rb
			ClearSwap(rb.culled_light_render_range, tmp.culled_light_render_range);//clear then swap the stuff back into rb
			ClearSwap(rb.canvas_render_range, tmp.canvas_render_range);//clear then swap the stuff back into rb

			std::swap(rb.d_lightpool, tmp.d_lightpool);//swap back then restart
			rb.d_lightpool.Restart();				   //

		};

		// todo: scenegraph traversal
		RenderBuffer& result = GetWriteBuffer();
		vector<mt::Future<void>> futures;
#if 0   //change to 0 to reduce reallocation count.
		result = RenderBuffer{};
#else
		reset_render_buffer(result);
#endif
		result.camera.reserve(cameras.size());

		result.renderer_vertex_shaders = renderer_vertex_shaders;
		result.renderer_fragment_shaders = renderer_fragment_shaders;

		// memcpy the lights until there is a smarter implementation
		result.lights.reserve(lights.size());
#define POST() futures.emplace_back(Core::GetThreadPool().Post([&](){
#define POST_END() }));

		POST()
			for (auto& camera : cameras)
			{
				if (camera.GetHandle().scene == Scene::prefab)
					continue;
				if (!camera.enabled || !camera.GetGameObject()->ActiveInHierarchy())
					continue;

				if (camera.GetHandle().scene == Scene::editor)
					result.curr_scene_camera_index = result.camera.size();
				result.camera.emplace_back(camera.GenerateCameraData());
			}
			for (auto& elem : lights)
			{
				if (isolate)
				{
					if (!elem.isolate)
						continue;
				}
				//result.light_camera_data.emplace_back(elem.GenerateCameraData());//Add the camera needed for the shadowmap
				if (elem.is_active_and_enabled())
				{
					auto res = elem.GenerateLightData();
					
					if (res.index == 1)
					{
						//Dtor now frees the shadow maps.
						//Core::GetSystem<SceneManager>().OnSceneChange += [&](RscHandle<Scene>) { 
						//
						//	//auto& e = std::get<DirectionalLight>(elem.light);							
						//	for (auto& elem : d_lightmaps)
						//	{
						//		for (auto& e : elem.second.cam_lightmaps)
						//		{
						//			e.light_map->attachments.clear();
						//			e.light_map->depth_attachment.reset();
						//			e.light_map->stencil_attachment.reset();
						//		}
						//		elem.second.cam_lightmaps.clear();
						//	}
						//	d_lightmaps.clear();
						//};

						//auto& e = std::get<DirectionalLight>(elem.light);
						//
						//for (auto& c : result.camera)
						//{
						//	result.d_lightmaps[c.obj_id].cam_lightmaps = result.d_lightpool.GetShadowMaps(elem);
						//}
					}
					result.lights.emplace_back(res);
				}
			}

			d_lightmaps = result.d_lightmaps;
			//result.d_lightmaps = d_lightmaps;
		POST_END();

		POST()
			alignas(machine::cache_line_sz) hash_table<Handle<Animator>, size_t> skeleton_indices;

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
		POST_END();

		POST()
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
		POST_END();

		POST()
			for (auto& elem : ps)
			{
				if (!elem.renderer.enabled || elem.data.num_alive == 0 || !elem.GetGameObject()->ActiveInHierarchy())
					continue;

				const auto sz = elem.data.num_alive;
				auto& render_data = result.particle_render_data.emplace_back();

				render_data.particles.resize(sz);

				auto decomposed = decompose(elem.transform);
				for (uint16_t i = 0; i < sz; ++i)
					render_data.particles[i].position = elem.data.position[i];
				if (!elem.main.in_world_space)
				{
					for (auto& p : render_data.particles)
						p.position = elem.transform * vec4{ p.position, 1.0f };
				}

				for (uint16_t i = 0; i < sz; ++i)
					render_data.particles[i].rotation = elem.data.rotation[i];
				for (uint16_t i = 0; i < sz; ++i)
					render_data.particles[i].size = elem.data.size[i];
				for (uint16_t i = 0; i < sz; ++i)
					render_data.particles[i].color = elem.data.color[i];

				render_data.material_instance = elem.renderer.material;
			}
		POST_END();

		POST()
			for (auto& f : fonts)
			{
				if (f.text.empty() || !f.font || !f.GetGameObject()->ActiveInHierarchy())
					continue;

				auto& render_data = result.font_render_data.emplace_back();
				const auto font_data = FontData::Generate(f.text, f.font, f.font_size, f.letter_spacing, f.line_height, f.alignment, 0);

				const float w = font_data.width;
				const float h = font_data.height;
				float ox = 0, oy = 0;

				switch (f.anchor)
				{
				case TextAnchor::UpperLeft: case TextAnchor::UpperCenter: case TextAnchor::UpperRight: oy = 0; break;
				case TextAnchor::MiddleLeft: case TextAnchor::MiddleCenter: case TextAnchor::MiddleRight: oy = h * 0.5f; break;
				case TextAnchor::LowerLeft: case TextAnchor::LowerCenter: case TextAnchor::LowerRight: oy = h; break;
				}

				switch (f.anchor)
				{
				case TextAnchor::UpperLeft: case TextAnchor::MiddleLeft: case TextAnchor::LowerLeft: 
					switch (f.alignment)
					{
					case TextAlignment::Left: ox = 0; break;
					case TextAlignment::Center: ox = w * 0.5f; break;
					case TextAlignment::Right: ox = w; break;
					}
					break;
				case TextAnchor::UpperCenter: case TextAnchor::MiddleCenter: case TextAnchor::LowerCenter:
					switch (f.alignment)
					{
					case TextAlignment::Left: ox = -w * 0.5f; break;
					case TextAlignment::Center: ox = 0; break;
					case TextAlignment::Right: ox = w * 0.5f; break;
					}
					break;
				case TextAnchor::UpperRight: case TextAnchor::MiddleRight: case TextAnchor::LowerRight:
					switch (f.alignment)
					{
					case TextAlignment::Left: ox = -w; break;
					case TextAlignment::Center: ox = -w * 0.5f; break;
					case TextAlignment::Right: ox = 0; break;
					}
					break;
				}

				render_data.coords = font_data.coords;
				render_data.color = f.color;
				render_data.transform = f.GetGameObject()->Transform()->GlobalMatrix() * translate(vec3{ ox, oy, 0 });
				render_data.atlas = f.font;
			}

			auto& ui = Core::GetSystem<UISystem>();
			for (auto& im : images)
			{
				if (!im.texture)
					continue;

				const auto& go = im.GetGameObject();
				if (!go->ActiveInHierarchy())
					continue;

				const auto canvas = ui.FindCanvas(go);
				if (!canvas)
				{
					LOG_WARNING_TO(LogPool::GAME, "Image must be child of Canvas.");
					continue;
				}

				const auto& rt = *go->GetComponent<RectTransform>();
				auto& render_data = result.ui_render_per_canvas[ui.FindCanvas(go)].emplace_back();

				auto sz = rt._local_rect.size * 0.5f;

				if (im.preserve_aspect)
				{
					const float tex_aspect = im.texture->AspectRatio();
					const float rt_aspect = rt._local_rect.size.x / rt._local_rect.size.y;
					if (tex_aspect > rt_aspect) // horizontally longer
						sz.y = sz.x / tex_aspect;
					else if (tex_aspect < rt_aspect) // vertically longer
						sz.x = sz.y * tex_aspect;
				}

				render_data.transform = rt._matrix * mat4{ scale(vec3{sz, 1.0f}) };
				render_data.material = im.material;
				render_data.color = im.tint;
				render_data.data = ImageData{ im.texture };
				render_data.depth = go->Transform()->Depth();
			}

			for (auto& text : texts)
			{
				if (text.text.empty() || !text.font)
					continue;

				const auto& go = text.GetGameObject();
				if(!go->ActiveInHierarchy())
					continue;

				const auto canvas = ui.FindCanvas(go);
				if (!canvas)
				{
					LOG_WARNING_TO(LogPool::GAME, "Text must be child of Canvas. (Use TextMesh otherwise)");
					continue;
				}

				const auto& rt = *go->GetComponent<RectTransform>();

				auto& render_data = result.ui_render_per_canvas[canvas].emplace_back();
				++canvas->num_of_text;

				constexpr auto anchor_to_alignment = [](TextAnchor anchor)
				{
					switch (anchor)
					{
					case TextAnchor::UpperLeft: case TextAnchor::MiddleLeft: case TextAnchor::LowerLeft:
						return TextAlignment::Left;
					case TextAnchor::UpperCenter: case TextAnchor::MiddleCenter: case TextAnchor::LowerCenter:
						return TextAlignment::Center;
					case TextAnchor::UpperRight: case TextAnchor::MiddleRight: case TextAnchor::LowerRight:
						return TextAlignment::Right;
					}
					return TextAlignment::Left;
				};

				const float sx = rt._local_rect.size.x;
				const float sy = rt._local_rect.size.y;

				const auto font_data = FontData::Generate(
					text.text, text.font,
					text.best_fit ? 0 : text.font_size,
					text.letter_spacing,
					text.line_height,
					anchor_to_alignment(text.alignment),
					text.wrap ? sx : 0);

				float tw = font_data.width;
				float th = font_data.height;

				render_data.material = text.material;
				render_data.color = text.color;
				render_data.data = TextData{ font_data.coords, text.font };
				render_data.depth = go->Transform()->Depth();

				float s = 1.0f;

				if (text.best_fit)
				{
					const float sw = sx / tw;
					const float sh = sy / th;
					s = sw > sh ? sh : sw;
					tw *= s;
					th *= s;
				}

				switch (text.alignment)
				{
				case TextAnchor::UpperLeft:    render_data.transform = rt._matrix * translate(vec3{ -0.5f * sx, 0.5f * sy, 0 }); break;
				case TextAnchor::MiddleLeft:   render_data.transform = rt._matrix * translate(vec3{ -0.5f * sx, 0.5f * th, 0 }); break;
				case TextAnchor::LowerLeft:    render_data.transform = rt._matrix * translate(vec3{ -0.5f * sx, -0.5f * sy + th, 0 }); break;
				case TextAnchor::UpperCenter:  render_data.transform = rt._matrix * translate(vec3{ 0, 0.5f * sy, 0 }); break;
				case TextAnchor::MiddleCenter: render_data.transform = rt._matrix * translate(vec3{ 0, 0.5f * th, 0 }); break;
				case TextAnchor::LowerCenter:  render_data.transform = rt._matrix * translate(vec3{ 0, -0.5f * sy + th, 0 }); break;
				case TextAnchor::UpperRight:   render_data.transform = rt._matrix * translate(vec3{ 0.5f * sx, 0.5f * sy, 0 }); break;
				case TextAnchor::MiddleRight:  render_data.transform = rt._matrix * translate(vec3{ 0.5f * sx, 0.5f * th, 0 }); break;
				case TextAnchor::LowerRight:   render_data.transform = rt._matrix * translate(vec3{ 0.5f * sx, -0.5f * sy + th, 0 }); break;
				}

				if (text.best_fit)
					render_data.transform = render_data.transform * mat4{ scale(vec3{ s, s, 1.0f }) };
			}
		POST_END()

		for (auto& elem : futures)
			elem.get();
		futures.clear();

		POST()
		{
			auto& unique_particles = result.particle_render_data;
			const size_t avg_particle_count = 100;
			const auto size = cameras.size() * unique_particles.size();
			result.particle_range.reserve(result.particle_range.size() + size);
			result.particle_buffer.reserve(result.particle_buffer.size() + size * avg_particle_count);
		}
		{
			auto& unique_fonts = result.font_render_data;
			const size_t avg_font_count = 100;
			const auto size = cameras.size() * unique_fonts.size();
			result.font_range.reserve(result.font_range.size() + size);
			result.font_buffer.reserve(result.font_buffer.size() + size * avg_font_count);
		}
			std::sort(result.skinned_mesh_render.begin(), result.skinned_mesh_render.end(), aro_inst_comp{});
		POST_END()

		POST()
			std::sort(result.mesh_render.begin(), result.mesh_render.end(), ro_inst_comp{});
		POST_END()

		POST()
#pragma region FOR UI
			//////For UI////////
			{
				auto& unique_fonts = texts;
				const size_t avg_font_count = 100;
				const auto size = result.ui_canvas.size() + unique_fonts.size();
				const auto size_2 = result.ui_render_per_canvas.size() + unique_fonts.size();
				result.ui_text_range.reserve(result.ui_text_range.size() + size);
				result.ui_text_buffer.reserve(result.ui_text_buffer.size() + size * avg_font_count);
				result.ui_text_buffer.reserve(result.ui_text_buffer.size() + size * avg_font_count);
				//result.ui_canvas_range.reserve(result.ui_text_buffer.size() + size * avg_font_count);
				//result.ui_text_buffer.reserve(result.ui_text_buffer.size() + size * avg_font_count);
			}

			// sort ui render by depth then z pos
			for (auto& [canvas, vec] : result.ui_render_per_canvas)
			{
				std::stable_sort(vec.begin(), vec.end(),
					[](const UIRenderObject& a, const UIRenderObject& b) {
						return a.depth == b.depth ?
							a.transform[3].z < b.transform[3].z :
							a.depth < b.depth;
					}
				);
			}

			//Push it into the char buffer when done
			result.ui_total_num_of_text = 0;
			for (auto& [canvas, vec] : result.ui_render_per_canvas)
			{
				//No need to cull, this is to find all the coords data and append them to one buffer
				//CanvasRenderRange range{};

				auto& res = result.ui_canvas.emplace_back();

				res.num_of_text = canvas->num_of_text;
				res.render_target = canvas->render_target;
				res.ui_ro = vec;
				for (auto& elem : vec)
				{
					std::visit([&](const auto& data)
					{
						using T = std::decay_t<decltype(data)>;
						if constexpr (!std::is_same_v<T, ImageData>)
						{
							ProcessCanvas(data.coords, result.ui_text_buffer, result.ui_text_range, result.ui_total_num_of_text);
							//result.canvas_render_range.emplace_back(range);
						}
					}, elem.data);
				}


			}
#pragma endregion
        POST_END()

        for (auto& elem : futures)
            elem.get();

		result.active_light_buffer.reserve(result.camera.size()+ result.lights.size());
		result.directional_light_buffer.reserve(result.camera.size());
		vector<sphere> bounding_vols;
		bounding_vols.resize(result.mesh_render.size());
		std::transform(result.mesh_render.begin(), result.mesh_render.end(), bounding_vols.begin(), [](const RenderObject& ro) { return ro.mesh->bounding_volume * ro.transform; });
		{
			vector<LightData> new_lights;
			new_lights.reserve(result.camera.size() * 2);
			for (auto& cam : result.camera)
			{
				auto& camera = cam;
				RenderRange range{ camera };
				{
					const auto [start_index, end_index] = CullAndBatchRenderObjects(camera, result.mesh_render, bounding_vols, result.instanced_mesh_render, result.inst_mesh_render_buffer);
					range.inst_mesh_render_begin = start_index;
					range.inst_mesh_render_end = end_index;
					ProcessParticles(result.particle_render_data, result.particle_buffer, result.particle_range, range);
					ProcessFonts(result.font_render_data, result.font_buffer, result.font_range, range);

					CullLights(camera, result.d_lightpool, result.lights, new_lights, result.active_light_buffer, result.directional_light_buffer, range);

				}
				result.culled_render_range.emplace_back(range);
				//{
			//	auto [start_index, end_index] = CullAndBatchAnimatedRenderObjects(frustum, result.skinned_mesh_render, result.instanced_skinned_mesh_render);
			//	range.inst_mesh_render_begin = start_index;
			//	range.inst_mesh_render_end = end_index;
			//}
			}
			//Disable the shadows of the non-cascaded directional lights
			for (auto& light : result.lights)
			{
				if (light.index == 1)
					light.cast_shadow = false;
			}
			//append the added lights at the back of the light range
			result.lights.insert(result.lights.end(), new_lights.begin(), new_lights.end());
		}
		for (auto& request : this->request_buffer)
		{
			auto& cam = request.data.camera;
			auto& out_instanced_mesh_render = result.instanced_mesh_render;
			auto& out_inst_mesh_render_buffer = result.inst_mesh_render_buffer;
			{
				const auto [start_index, end_index] = CullAndBatchRenderObjects(cam, result.mesh_render, bounding_vols, out_instanced_mesh_render, out_inst_mesh_render_buffer,&request.data.handles);
				request.data.inst_mesh_render_begin = start_index;
				request.data.inst_mesh_render_end = end_index;
			}
		}
		size_t i = 0;
		

		auto draw_frustum = [this](const frustum& frust, color col,seconds duration,bool depth_test=true)
		{

			auto dbg_cascade_name = "debug_cascade";
			auto active_opt = this->extra_vars.Get<bool>(dbg_cascade_name);

			if (!active_opt)
			{
				extra_vars.Set(dbg_cascade_name,false);
				active_opt = false;
			}
			if (!*active_opt) return;

			auto intersection_point = [](halfspace a, halfspace b, halfspace c)
			{
				mat3 mat = mat3{ a.normal,b.normal,c.normal };
				return mat.transpose().inverse() * vec3 { -a.dist, -b.dist, -c.dist };
			};
			vec3 points[8] =
			{
				/*vec3{-1, 1,-1},//*/intersection_point(frust.sides[FrustumSide::Near],frust.sides[FrustumSide::Up  ],frust.sides[FrustumSide::Left ]),
				/*vec3{ 1, 1,-1},//*/intersection_point(frust.sides[FrustumSide::Near],frust.sides[FrustumSide::Up  ],frust.sides[FrustumSide::Right]),
				/*vec3{ 1,-1,-1},//*/intersection_point(frust.sides[FrustumSide::Near],frust.sides[FrustumSide::Down],frust.sides[FrustumSide::Right]),
				/*vec3{-1,-1,-1},//*/intersection_point(frust.sides[FrustumSide::Near],frust.sides[FrustumSide::Down],frust.sides[FrustumSide::Left ]),
				/*vec3{-1, 1, 1},//*/intersection_point(frust.sides[FrustumSide::Far ],frust.sides[FrustumSide::Up  ],frust.sides[FrustumSide::Left ]),
				/*vec3{ 1, 1, 1},//*/intersection_point(frust.sides[FrustumSide::Far ],frust.sides[FrustumSide::Up  ],frust.sides[FrustumSide::Right]),
				/*vec3{ 1,-1, 1},//*/intersection_point(frust.sides[FrustumSide::Far ],frust.sides[FrustumSide::Down],frust.sides[FrustumSide::Right]),
				/*vec3{-1,-1, 1},//*/intersection_point(frust.sides[FrustumSide::Far ],frust.sides[FrustumSide::Down],frust.sides[FrustumSide::Left ]),
			};
			//Yes I'm too scrub to write some smart code to do this for me.
			Core::GetSystem<DebugRenderer>().Draw(points[0    ], points[4    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[1    ], points[5    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[2    ], points[6    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[3    ], points[7    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[0    ], points[1    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[1    ], points[2    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[2    ], points[3    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[3    ], points[0    ], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[4 + 0], points[4 + 1], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[4 + 1], points[4 + 2], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[4 + 2], points[4 + 3], col, duration, depth_test);
			Core::GetSystem<DebugRenderer>().Draw(points[4 + 3], points[4 + 0], col, duration, depth_test);

		};
		size_t derp = 0;
		for (auto& light : result.lights)
		{
			CameraData light_cam_info{};
			light_cam_info.view_matrix = { light.v };
			light_cam_info.projection_matrix = { light.p };
			LightRenderRange range{ i++ };
			// TODO: Cull cascaded directional light
			size_t lm_i = 0;
			if (light.index == 1)
			{
				for (auto& lightmap : light.light_maps)
				{
					range.light_map_index = lm_i;
					{
						if (!light.cast_shadow)
						{
							range.inst_mesh_render_begin = range.inst_mesh_render_end = 0;
						}
						else
						{
							light_cam_info.projection_matrix = { lightmap.cascade_projection };
							const auto frust = camera_vp_to_frustum(light_cam_info.projection_matrix * light_cam_info.view_matrix);
							draw_frustum(frust, color{ ((float)++derp) / result.camera.size(),0,(lm_i + 1.0f) / light.light_maps.size(),1 }, {});

							const auto [start_index, end_index] = CullAndBatchRenderObjects(light_cam_info, result.mesh_render, bounding_vols, result.instanced_mesh_render, result.inst_mesh_render_buffer);
							range.inst_mesh_render_begin = start_index;
							range.inst_mesh_render_end = end_index;
						}
					}
					result.culled_light_render_range.emplace_back(range);
					++lm_i;
				}
			}
			else
			{
				for (auto& lightmap : light.light_maps)
				{
					range.light_map_index = lm_i;
					{
						if (!light.cast_shadow)
						{
							range.inst_mesh_render_begin = range.inst_mesh_render_end = 0;
						}
						else
						{
							const auto [start_index, end_index] = CullAndBatchRenderObjects(light_cam_info, result.mesh_render, bounding_vols, result.instanced_mesh_render, result.inst_mesh_render_buffer);
							range.inst_mesh_render_begin = start_index;
							range.inst_mesh_render_end = end_index;
						}
					}
					result.culled_light_render_range.emplace_back(range);
					++lm_i;
				}
			}
			//{
			//	auto [start_index, end_index] = CullAndBatchAnimatedRenderObjects(frustum, result.skinned_mesh_render, result.instanced_skinned_mesh_render);
			//	range.inst_mesh_render_begin = start_index;
			//	range.inst_mesh_render_end = end_index;
			//}
		}

#ifdef FLATTEN_UNI //This block is for when we wanna flatten the uniforms
		for (auto& inst : result.instanced_mesh_render)
		{
			MaterialInstToUniforms(*inst.material_instance, inst.uniform_buffers, inst.uniform_textures);
		}
#endif
		{
			auto& instanced_ros = result.instanced_mesh_render;
			auto& [inst_mesh_start, inst_mesh_end] = result.skinned_inst_range;
			auto num_skinned = result.skinned_mesh_render.size();
			std::pair<size_t, size_t> ani_buffer_range = { result.inst_mesh_render_buffer.size(),result.inst_mesh_render_buffer.size() + num_skinned};

			inst_mesh_start = result.instanced_mesh_render.size();
			inst_mesh_end = inst_mesh_start + num_skinned;

			
			instanced_ros.reserve(inst_mesh_end);
			result.inst_mesh_render_buffer.reserve(ani_buffer_range.second);
			auto pani_handles = (request_buffer.size()) ? std::make_shared<vector<ColorPickResult::result_t>>() : shared_ptr<vector<ColorPickResult::result_t>>{};
			if (pani_handles)
				pani_handles->reserve(num_skinned);
			for (auto& ani_obj : result.skinned_mesh_render)
			{
				if (pani_handles)
				{
					auto& ani_handles = *pani_handles;
					ani_handles.emplace_back(ani_obj.obj_id);

				}
				[[maybe_unused]]auto& i_ro = instanced_ros.emplace_back(CreateIROInfo(ani_obj));
				i_ro.instanced_index = result.inst_mesh_render_buffer.size();
				i_ro.num_instances = 1;
				result.inst_mesh_render_buffer.emplace_back(InstancedData{ani_obj.transform});
#ifdef FLATTEN_UNI
				AniRenderObjectToUniforms(result.skeleton_transforms, ani_obj, i_ro.uniform_buffers, i_ro.uniform_textures);
#else
				SkeletonToUniforms(result.skeleton_transforms[ani_obj.skeleton_index], i_ro.uniform_buffers);
#endif
			}

			for (auto& request : request_buffer)
			{
				request.data.inst_skinned_mesh_render_begin = result.skinned_inst_range.first;
				request.data.inst_skinned_mesh_render_end = result.skinned_inst_range.second;
				request.data.ani_handles = pani_handles;
			}
		}
		//SubmitBuffers(std::move(result));
		SwapWritingBuffer();
	}
	RscHandle<ShaderProgram> LoadShader(string_view path, bool reload=true)
	{
		PathHandle hpath = path;
		auto result = Core::GetResourceManager().Load<ShaderProgram>(hpath,reload);

		if (!result)
		{
			if (hpath)
			{
				string fullpath  = hpath.GetFullPath();
				string mountpath = hpath.GetMountPath();
				LOG_TO(LogPool::GFX, "Failed to load resource %s\n mounted on %s", fullpath.c_str(), mountpath.c_str());
			}
			else
			{
				string mountpath{ path }; //make sure to handle when a string_view isn't terminated by the null character.
				LOG_TO(LogPool::GFX, "Failed to load resource on mount path %s", mountpath.c_str());
			}
		}
		return *result;
	}

	void GraphicsSystem::LoadShaders()
	{
		LoadShaderImpl();

		//auto dir = Core::GetSystem<FileSystem>().GetDir("/engine_data/shaders");
		//auto glsls = dir.GetFilesWithExtension(".glsl");
		//for (auto& glsl : glsls)
		//{
		//	Core::GetResourceManager().Load<ShaderSnippet>(glsl, false);
		//}
		///////////////////////Load vertex shaders
		//renderer_vertex_shaders[VDebug] = LoadShader("/engine_data/shaders/debug.vert");
		renderer_vertex_shaders[VNormalMesh] = LoadShader("/engine_data/shaders/mesh.vert");
		renderer_vertex_shaders[VNormalMeshPicker] = LoadShader("/engine_data/shaders/mesh_picking.vert");
		renderer_vertex_shaders[VParticle] = LoadShader("/engine_data/shaders/particle.vert");
		renderer_vertex_shaders[VSkinnedMesh] = LoadShader("/engine_data/shaders/skinned_mesh.vert");
		renderer_vertex_shaders[VSkinnedMeshPicker] = LoadShader("/engine_data/shaders/skinned_mesh_picking.vert");
		renderer_vertex_shaders[VSkyBox] = LoadShader("/engine_data/shaders/skybox.vert");
		renderer_vertex_shaders[VPBRConvolute] = LoadShader("/engine_data/shaders/pbr_convolute.vert");
		renderer_vertex_shaders[VFsq] = LoadShader("/engine_data/shaders/fsq.vert");
		renderer_vertex_shaders[VFont] = LoadShader("/engine_data/shaders/font.vert");
		renderer_vertex_shaders[VUi] = LoadShader("/engine_data/shaders/ui.vert");


		renderer_fragment_shaders[FDebug] = LoadShader("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FSkyBox] = LoadShader("/engine_data/shaders/skybox.frag");
		renderer_fragment_shaders[FShadow] = LoadShader("/engine_data/shaders/shadow.frag");
		renderer_fragment_shaders[FPicking] = LoadShader("/engine_data/shaders/picking.frag");
		////////////////////Load fragment Shaders
		//renderer_fragment_shaders[FDebug] = LoadShader("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FPBRConvolute] = LoadShader("/engine_data/shaders/pbr_convolute.frag");
		renderer_fragment_shaders[FSkyBox] = LoadShader("/engine_data/shaders/skybox.frag");
		renderer_fragment_shaders[FBrdf] = LoadShader("/engine_data/shaders/brdf.frag");
		renderer_fragment_shaders[FFont] = LoadShader("/engine_data/shaders/font.frag");
		renderer_fragment_shaders[FDeferredPost] = LoadShader("/engine_data/shaders/deferred_post.frag");
		renderer_fragment_shaders[FDeferredPostSpecular] = LoadShader("/engine_data/shaders/deferred_post_specular.frag");
		renderer_fragment_shaders[FDeferredPostAmbient] = LoadShader("/engine_data/shaders/deferred_post_ambient.frag");
		renderer_fragment_shaders[FDeferredHDR] = LoadShader("/engine_data/shaders/deferred_hdr.frag");

		////////////////////Load geometry Shaders
		renderer_geometry_shaders[GSinglePassCube] = LoadShader("/engine_data/shaders/single_pass_cube.geom");


	}

	void GraphicsSystem::MaterialInstToUniforms(const MaterialInstance& , hash_table<string, string>& , hash_table<string, RscHandle<Texture>>& )
	{
	}

	void GraphicsSystem::AniRenderObjectToUniforms(const vector<SkeletonTransforms>& skeletons, const AnimatedRenderObject& ani_ro, hash_table<string, string>& uniform_buffers, hash_table<string, RscHandle<Texture>>& uniform_textures)
	{
		MaterialInstToUniforms(*ani_ro.material_instance, uniform_buffers, uniform_textures);
		SkeletonToUniforms(skeletons[ani_ro.skeleton_index], uniform_buffers);
	}

	void idk::GraphicsSystem::SkeletonToUniforms(const SkeletonTransforms& , hash_table<string, string>& )
	{
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