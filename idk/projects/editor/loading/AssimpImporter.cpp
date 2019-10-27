#include "pch.h"
#include "AssimpImporter.h"
#include "AssimpImporter_helpers.h"
#undef min
#undef max

//* // tmp include
#include <scene/sceneManager.h>
#include <file/FileSystem.h>
#include <anim/AnimationSystem.h>
#include <core/GameObject.h>
#include <prefab/Prefab.h>
#include <prefab/PrefabUtility.h>
#include <gfx/ShaderGraph.h>
#include <common/Transform.h>
#include <idk/gfx/MeshRenderer.h>
#include <anim/SkinnedMeshRenderer.h>
#include <anim/Animator.h>
//*/
#include <iostream>

namespace idk
{

	const char* AssimpImporter::root_bone_keyword = "_root_";
	const char* AssimpImporter::bone_end_keyword = "_end_";
	const char* AssimpImporter::bone_include_keyword = "_b_include_";
	const char* AssimpImporter::bone_exclude_keyword = "_b_exclude_";

	struct AssimpImporter::Data
	{
		vkn::MeshModder _modder;
	};

	ResourceBundle AssimpImporter::LoadFile(PathHandle path_to_resource, const MetaBundle& meta_bundle)
	{
		ResourceBundle ret_val;

		ai_helpers::Scene importer_scene;
		const bool success = ai_helpers::Import(importer_scene, path_to_resource);
		IDK_ASSERT_MSG(success, "[AssimpImporter] Assimp failed to load file.");
		if (!success)
			return ret_val;

		vector<RscHandle<Mesh>> mesh_handles;
		RscHandle<anim::Skeleton> skeleton_handle;
		vector<RscHandle<anim::Animation>> animation_handles;

		ai_helpers::CompileMeshes(importer_scene, importer_scene.ai_scene->mRootNode);
		ai_helpers::CompileBones(importer_scene);
		ai_helpers::BuildSkeleton(importer_scene);

		// Building and adding all the meshes
		if (importer_scene.has_meshes)
		{
			// For opengl
			vector<ai_helpers::Vertex> vertices;

			// For vulkan
			vector<vec3	>	positions;
			vector<vec3	>	normals;
			vector<vec2	>	uvs;
			vector<vec3	>	tangents;
			vector<vec3	>	bi_tangents;
			vector<ivec4>	bone_ids;
			vector<vec4	>	bone_weights;
			auto& mesh_modder = _data->_modder;

			// For both
			vector<unsigned> indices;

			for (const auto& elem : importer_scene.meshes)
			{
				if (Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL)
				{
					RscHandle<ogl::OpenGLMesh> mesh_handle;
					{
						auto rsc_exists = meta_bundle.FetchMeta<Mesh>(elem->mName.data);
						if (rsc_exists)
							mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLMesh>(rsc_exists->guid);
						else
							mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLMesh>();
					}
					mesh_handle->Name(elem->mName.data);
					
					// Buffers are cleared when they enter this function
					ai_helpers::WriteToVertices(importer_scene, elem, vertices, indices);
					// Building the opengl buffers and descriptors
					ai_helpers::BuildMeshOpenGL(importer_scene, vertices, indices, mesh_handle);

					mesh_handles.emplace_back(mesh_handle);
					ret_val.Add(mesh_handle);
				}
				else
				{
					RscHandle<vkn::VulkanMesh> mesh_handle;
					{
						auto rsc_exists = meta_bundle.FetchMeta<Mesh>(elem->mName.data);
						if (rsc_exists)
							mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<vkn::VulkanMesh>(rsc_exists->guid);
						else
							mesh_handle = Core::GetResourceManager().LoaderEmplaceResource<vkn::VulkanMesh>();
					}
					mesh_handle->Name(elem->mName.data);

					// Collect the mesh data
					ai_helpers::WriteToBuffers(importer_scene, elem,
						positions, normals, uvs, tangents, bi_tangents, bone_ids, bone_weights, indices);
					// Building the vulkan mesh data
					ai_helpers::BuildMeshVulknan(importer_scene, mesh_modder, mesh_handle,
						positions, normals, uvs, tangents, bi_tangents, bone_ids, bone_weights, indices);

					mesh_handles.emplace_back(mesh_handle);
					ret_val.Add(mesh_handle);
				}
			}
		}

		// Adding skeleton if it exists
		if (importer_scene.has_skeleton)
		{
			// initializing the handle
			// RscHandle<anim::Skeleton> skeleton_handle;
			{
				auto search_res = meta_bundle.FetchMeta<anim::Skeleton>(importer_scene.final_skeleton[0]._name);
				if (search_res)
					skeleton_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Skeleton>(search_res->guid);
				else
					skeleton_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Skeleton>();
			}
			auto& skeleton = *skeleton_handle;

			skeleton = anim::Skeleton{ importer_scene.final_skeleton, importer_scene.final_skeleton_table };
			skeleton.Name(importer_scene.final_skeleton[0]._name);

			ret_val.Add(skeleton_handle);
		}

		if (importer_scene.has_animation)
		{
			ai_helpers::CompileAnimations(importer_scene);
			ai_helpers::BuildAnimations(importer_scene);

			for (auto& compiled_animation : importer_scene.compiled_clips)
			{
				// Initialize the name. Name will be the file name if there are no other resources besides the animation.
				string name;
				if (importer_scene.has_animation && !importer_scene.has_skeleton)
				{
					name = path_to_resource.GetStem();
				}
				else
					name = compiled_animation.name;

				// Initialize the resource handle
				RscHandle<anim::Animation> anim_clip_handle;
				{
					auto search_res = meta_bundle.FetchMeta<anim::Animation>(name);
					if (search_res)
						anim_clip_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Animation>(search_res->guid);
					else
						anim_clip_handle = Core::GetResourceManager().LoaderEmplaceResource<anim::Animation>();
				}

				auto& anim_clip = *anim_clip_handle;
				anim_clip.Name(name);

				anim_clip.SetSpeeds(compiled_animation.fps, compiled_animation.duration, compiled_animation.num_ticks);

				for (auto& animated_bone : compiled_animation.animated_bones)
				{
					anim_clip.AddAnimatedBone(animated_bone);
				}

				ret_val.Add(anim_clip_handle);
			}

		}
		// Here we decide whether or not to save as a prefab.
		// If importer_scene has no skeleton, then we only save as prefab if there is more than 1 mesh
		// If importer_scene has animation but no meshes, then we save the importer_scene as just an animation asset with no skeleton.
		// Check for errors here as well. Eg: Has skeleton but no mesh and no animation (Unlikely).

		// Check if this is a pure animation file. If it is, we don't even bother creating the prefab.
		if (importer_scene.has_animation && !importer_scene.has_skeleton)
		{
			return ret_val;
		}

		const auto scene = Core::GetSystem<SceneManager>().GetPrefabScene();
		const auto prefab_root = scene->CreateGameObject();
		prefab_root->Name(path_to_resource.GetStem());
		if (importer_scene.has_meshes && importer_scene.has_skeleton)
		{
			for (auto& handle : mesh_handles)
			{
				const auto mesh_child = scene->CreateGameObject();
				mesh_child->Name(handle->Name());
				mesh_child->Transform()->SetParent(prefab_root);
				const auto mesh_renderer = mesh_child->AddComponent<SkinnedMeshRenderer>();
				mesh_renderer->mesh = RscHandle<Mesh>{ handle };
			}
			// Add an animator component to the prefab so that the skinned mesh renderer can draw.
			// Need to revisit this as I feel this is the wrong way to do this.
			const auto animator = prefab_root->AddComponent<Animator>();
			animator->skeleton = skeleton_handle;
			Core::GetSystem<AnimationSystem>().GenerateSkeletonTree(*animator);

			for (auto& anim : animation_handles)
				animator->AddAnimation(anim);
		}
		else if (importer_scene.has_meshes && !importer_scene.has_skeleton)
		{
			// If there is only 1 mesh, we attach it to the prefab itself. If not, we make child objects with MeshRenderer components.
			if (importer_scene.num_meshes == 1)
			{
				// Just attach the mesh to the prefab itself
				auto mesh_renderer = prefab_root->AddComponent<MeshRenderer>();
				mesh_renderer->mesh = mesh_handles[0];
			}
			else
			{
				for (auto& handle : mesh_handles)
				{
					const auto mesh_child = scene->CreateGameObject();
					mesh_child->Name(handle->Name());
					mesh_child->Transform()->SetParent(prefab_root);
					const auto mesh_renderer = mesh_child->AddComponent<MeshRenderer>();
					mesh_renderer->mesh = RscHandle<Mesh>{ handle };
				}
			}
		}
		
		{
			auto prefab_meta = meta_bundle.FetchMeta<Prefab>();
			const auto prefab_handle = prefab_meta 
				? PrefabUtility::Create(prefab_root, prefab_meta->guid) 
				: PrefabUtility::Create(prefab_root);
			prefab_handle->Name(path_to_resource.GetStem());
			ret_val.Add(prefab_handle);
		}
		scene->DestroyGameObject(prefab_root);
		return ret_val;
	}

	AssimpImporter::AssimpImporter()
	{
		if (Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::Vulkan)
			_data = std::make_unique<Data>();
	}
	AssimpImporter::~AssimpImporter()
	{
	}
}