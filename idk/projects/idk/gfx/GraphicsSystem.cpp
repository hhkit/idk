#include "stdafx.h"
#include "GraphicsSystem.h"
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

	ColorPickResult GraphicsSystem::ColorPick(CameraData camera)
	{
		auto& req = request_stack.emplace_front();
		
		return req.promise(camera);
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
	std::pair<size_t,size_t> CullAndBatchRenderObjects(const CameraData& camera,const vector<RenderObject>& ro,const vector<sphere>& bounding_vols, vector<InstRenderObjects>& inst, vector<InstancedData>& instanced_data)
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

	void CullLights(const CameraData& camera,const vector<LightData>& lights, vector<size_t>& active_light_buffer,GraphicsSystem::RenderRange& range)
	{
		auto frustum = camera_vp_to_frustum(camera.projection_matrix * camera.view_matrix);
		range.light_begin = active_light_buffer.size();
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
				}
				break;
			case index_in_variant_v<SpotLight, LightVariant>:
			{
				auto bounding_box = camera_vp_to_bounding_box(light.vp);
				auto sphere = bounding_box_to_loose_sphere(bounding_box);
				if (frustum.contains(sphere))
				{
					active_light_buffer.emplace_back(i);
				}
			}
				break;
			case index_in_variant_v<DirectionalLight, LightVariant>:
				active_light_buffer.emplace_back(i);
				break;
			}
			if (light.index == index_in_variant_v<DirectionalLight, LightVariant>)
			{
				active_light_buffer.emplace_back(i);
				continue; //SKIP
			}

		}
		range.light_end = active_light_buffer.size();
	}

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
			ClearSwap(rb.font_render_data, tmp.font_render_data);//clear then swap the stuff back into rb
			ClearSwap(rb.font_range, tmp.font_range);
			ClearSwap(rb.font_buffer, tmp.font_buffer);
			ClearSwap(rb.ui_render_per_canvas, tmp.ui_render_per_canvas);
			ClearSwap(rb.ui_canvas, tmp.ui_canvas);
			ClearSwap(rb.ui_text_buffer, tmp.ui_text_buffer);
			//ClearSwap(rb.ui_text_buffer.pos1, tmp.ui_text_buffer.pos1);
			//ClearSwap(rb.ui_text_buffer.uv1, tmp.ui_text_buffer.uv1);
			//ClearSwap(rb.ui_text_buffer, tmp.ui_text_buffer);
			ClearSwap(rb.ui_text_range, tmp.ui_text_range);
			//ClearSwap(rb.ui_canvas_range, tmp.ui_canvas_range);
			ClearSwap(rb.instanced_mesh_render, tmp.instanced_mesh_render);//clear then swap the stuff back into rb
			//ClearSwap(rb.instanced_skinned_mesh_render  ,tmp.instanced_skinned_mesh_render  );//clear then swap the stuff back into rb
			ClearSwap(rb.inst_mesh_render_buffer, tmp.inst_mesh_render_buffer);//clear then swap the stuff back into rb
			//ClearSwap(rb.inst_skinned_mesh_render_buffer,tmp.inst_skinned_mesh_render_buffer);//clear then swap the stuff back into rb
			ClearSwap(rb.lights, tmp.lights);//clear then swap the stuff back into rb
			ClearSwap(rb.light_camera_data, tmp.light_camera_data);//clear then swap the stuff back into rb
			ClearSwap(rb.mesh_render, tmp.mesh_render);//clear then swap the stuff back into rb
			ClearSwap(rb.skinned_mesh_render, tmp.skinned_mesh_render);//clear then swap the stuff back into rb
			ClearSwap(rb.particle_render_data, tmp.particle_render_data);//clear then swap the stuff back into rb
			ClearSwap(rb.particle_buffer, tmp.particle_buffer);
			ClearSwap(rb.particle_range, tmp.particle_range);
			ClearSwap(rb.skeleton_transforms, tmp.skeleton_transforms);//clear then swap the stuff back into rb
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
			for (auto& elem : lights)
			{
				if (isolate)
				{
					if (!elem.isolate)
						continue;
				}
				//result.light_camera_data.emplace_back(elem.GenerateCameraData());//Add the camera needed for the shadowmap
				if (elem.is_active_and_enabled())
					result.lights.emplace_back(elem.GenerateLightData());
			}
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
			for (auto& camera : cameras)
			{
				if (camera.GetHandle().scene == Scene::prefab)
					continue;
				if (!camera.enabled)
					continue;

				if (camera.GetHandle().scene == Scene::editor)
					result.curr_scene_camera_index = result.camera.size();
				result.camera.emplace_back(camera.GenerateCameraData());
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
		POST_END();

		POST()
			for (auto& f : fonts)
			{
				if (f.text != "" && f.font)
				{
					auto& render_data = result.font_render_data.emplace_back();

					render_data.coords = FontData::Generate(f.text, f.font, f.font_size, f.letter_spacing, f.line_height, TextAlignment::Left, 0).coords;

					render_data.color = f.color;
					render_data.transform = f.GetGameObject()->Transform()->GlobalMatrix();
					render_data.atlas = f.font;
				}
			}

		auto& ui = Core::GetSystem<UISystem>();
		for (auto& im : images)
		{
			const auto& go = im.GetGameObject();
			const auto& rt = *go->GetComponent<RectTransform>();

			const auto canvas = ui.FindCanvas(go);
			if (!canvas)
			{
				LOG_WARNING_TO(LogPool::GAME, "Image must be child of Canvas.");
				continue;
			}

			auto& render_data = result.ui_render_per_canvas[ui.FindCanvas(go)].emplace_back();

			render_data.transform = rt._matrix *
				mat4{ scale(vec3{rt._local_rect.size * 0.5f, 1.0f}) };
			render_data.material = im.material;
			render_data.color = im.tint;
			render_data.data = ImageData{ im.texture };
			render_data.depth = go->Transform()->Depth();
		}

		for (auto& text : texts)
		{
			if (text.text != "" && text.font)
			{
				const auto& go = text.GetGameObject();
				const auto& rt = *go->GetComponent<RectTransform>();

				const auto canvas = ui.FindCanvas(go);
				if (!canvas)
				{
					LOG_WARNING_TO(LogPool::GAME, "Text must be child of Canvas. (Use TextMesh otherwise)");
					continue;
				}

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

		result.active_light_buffer.reserve(result.camera.size()* result.lights.size());
		vector<sphere> bounding_vols;
		bounding_vols.resize(result.mesh_render.size());
		std::transform(result.mesh_render.begin(), result.mesh_render.end(), bounding_vols.begin(), [](const RenderObject& ro) { return ro.mesh->bounding_volume * ro.transform; });
		for (auto& cam : result.camera)
		{
			auto& camera = cam;
			RenderRange range{ camera };
			{
				const auto [start_index, end_index] = CullAndBatchRenderObjects(camera, result.mesh_render,bounding_vols, result.instanced_mesh_render,result.inst_mesh_render_buffer);
				range.inst_mesh_render_begin = start_index;
				range.inst_mesh_render_end = end_index;
				ProcessParticles(result.particle_render_data, result.particle_buffer, result.particle_range,range);
				ProcessFonts(result.font_render_data,result.font_buffer,result.font_range,range);

				CullLights(camera,result.lights, result.active_light_buffer,range);

			}
			
			result.culled_render_range.emplace_back(range);
			//{
			//	auto [start_index, end_index] = CullAndBatchAnimatedRenderObjects(frustum, result.skinned_mesh_render, result.instanced_skinned_mesh_render);
			//	range.inst_mesh_render_begin = start_index;
			//	range.inst_mesh_render_end = end_index;
			//}
		}
		for (auto& request : this->request_buffer)
		{
			auto& cam = request.data.camera;
			auto& out_instanced_mesh_render = result.instanced_mesh_render;
			auto& out_inst_mesh_render_buffer = result.inst_mesh_render_buffer;
			{
				const auto [start_index, end_index] = CullAndBatchRenderObjects(cam, result.mesh_render, bounding_vols, out_instanced_mesh_render, out_inst_mesh_render_buffer);
				request.data.inst_mesh_render_begin = start_index;
				request.data.inst_mesh_render_end = end_index;
			}
		}
		size_t i = 0;
		
		for (auto& light : result.lights)
		{
			CameraData camera{};
			camera.view_matrix = { light.v };
			camera.projection_matrix = { light.p };
			LightRenderRange range{ ++i };
			{
				if (!light.cast_shadow)
				{
					range.inst_mesh_render_begin = range.inst_mesh_render_end = 0;
				}
				else
				{
					const auto [start_index, end_index] = CullAndBatchRenderObjects(camera, result.mesh_render, bounding_vols, result.instanced_mesh_render, result.inst_mesh_render_buffer);
					range.inst_mesh_render_begin = start_index;
					range.inst_mesh_render_end = end_index;
				}
			}
			result.culled_light_render_range.emplace_back(range);
			//{
			//	auto [start_index, end_index] = CullAndBatchAnimatedRenderObjects(frustum, result.skinned_mesh_render, result.instanced_skinned_mesh_render);
			//	range.inst_mesh_render_begin = start_index;
			//	range.inst_mesh_render_end = end_index;
			//}
		}

		//SubmitBuffers(std::move(result));
		SwapWritingBuffer();
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
		//renderer_vertex_shaders[VDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.vert");
		renderer_vertex_shaders[VNormalMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/mesh.vert");
		renderer_vertex_shaders[VNormalMeshPicker] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/mesh_noinstance.vert");
		renderer_vertex_shaders[VParticle] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/particle.vert");
		renderer_vertex_shaders[VSkinnedMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skinned_mesh.vert");
		renderer_vertex_shaders[VSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.vert");
		renderer_vertex_shaders[VPBRConvolute] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.vert");
		renderer_vertex_shaders[VFsq] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/fsq.vert");
		renderer_vertex_shaders[VFont] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/font.vert");
		renderer_vertex_shaders[VUi] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/ui.vert");


		renderer_fragment_shaders[FDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.frag");
		renderer_fragment_shaders[FShadow] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/shadow.frag");
		renderer_fragment_shaders[FPicking] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/picking.frag");
		////////////////////Load fragment Shaders
		//renderer_fragment_shaders[FDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FPBRConvolute] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.frag");
		renderer_fragment_shaders[FSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.frag");
		renderer_fragment_shaders[FBrdf] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/brdf.frag");
		renderer_fragment_shaders[FFont] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/font.frag");
		renderer_fragment_shaders[FDeferredPost] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_post.frag");
		renderer_fragment_shaders[FDeferredPostSpecular] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_post_specular.frag");
		renderer_fragment_shaders[FDeferredPostAmbient] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_post_ambient.frag");

		////////////////////Load geometry Shaders
		renderer_geometry_shaders[GSinglePassCube] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/single_pass_cube.geom");


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