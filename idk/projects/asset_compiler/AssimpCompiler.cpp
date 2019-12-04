
#include <idk.h>
#include <scene/Scene.h>
#include <idk_config.h>
#include <../idk/gfx/Mesh.h>
#include <res/ResourceHandle.h>
#include <res/MetaBundle.h>

#include "AssimpCompiler.h"
#include "AssimpCompiler_helpers.h"

namespace idk 
{
	opt<AssetBundle> idk::AssimpCompiler::LoadAsset(string_view full_path, const MetaBundle& bundle)
	{
		const fs::path fs_path{ full_path };

		string err_msg;
		ai_helpers::Scene importer_scene;
		const bool success = importer_scene.Import(fs_path);

		if (!success || importer_scene.ai_scene == nullptr)
		{
			err_msg = string{ "Importing " } + full_path.data() + " result: FAILED";
			LOG_ERROR_TO(LogPool::ANIM, err_msg);
			return {};
		}

		err_msg = string{ "Importing " } + full_path.data() + " result: SUCCESS";
		LOG_TO(LogPool::ANIM, err_msg);

		vector<AssetBundle::AssetPair> generated_resources;
		MetaBundle updated_metas;
		
		vector<RscHandle<Mesh>> mesh_handles;
		RscHandle<anim::Skeleton> skeleton_handle;
		vector<RscHandle<anim::Animation>> animation_handles;

		importer_scene.CollectMeshes(importer_scene.ai_scene->mRootNode);
		importer_scene.CollectBones();
		importer_scene.BuildSkeleton();

		// Building and adding all the meshes
		if (importer_scene.has_meshes)
		{
			// Build all the meshes
			auto mesh_datas{ importer_scene.BuildMeshBuffers() };

			// Compile and save mesh
			for (auto& mesh_data : mesh_datas)
			{
				CompiledMesh mesh = importer_scene.CompileMesh(mesh_data);
				auto [t_guid, t_meta] = [&]()
				{
					auto res = bundle.FetchMeta<Mesh>(mesh_data.name);
					auto guid = res ? res->guid : Guid::Make();
					return res ? std::make_pair(guid, *res) : std::make_pair(guid, SerializedMeta{ guid, mesh_data.name, string{reflect::get_type<Mesh>().name()}, "" } );
				}();

				updated_metas.metadatas.emplace_back(t_meta);
				generated_resources.emplace_back(t_guid, std::move(mesh));
			}
		}

		// Adding skeleton if it exists
		if (importer_scene.has_skeleton)
		{
			anim::Skeleton skeleton = importer_scene.CompileSkeleton();
			auto [t_guid, t_meta] = [&]()
			{
				auto res = bundle.FetchMeta<anim::Skeleton>();
				auto guid = res ? res->guid : Guid::Make();
				return res ? std::make_pair(guid, *res) : std::make_pair(guid, SerializedMeta{ guid, skeleton.Name().data(), string{reflect::get_type<anim::Skeleton>().name()}, "" });
			}();

			updated_metas.metadatas.emplace_back(t_meta);
			generated_resources.emplace_back(t_guid, std::move(skeleton));
		}

		if (importer_scene.has_animation)
		{
			auto compiled_animations{ importer_scene.CompileAnimations() };
			if(compiled_animations.size() == 1)
				compiled_animations[0].Name(fs_path.stem().generic_string());

			for (auto& animation : compiled_animations)
			{
				auto [t_guid, t_meta] = [&]()
				{
					auto res = bundle.FetchMeta<anim::Animation>(animation.Name());
					auto guid = res ? res->guid : Guid::Make();
					return res ? std::make_pair(guid, *res) : std::make_pair(guid, SerializedMeta{ guid, animation.Name().data(), string{reflect::get_type<anim::Animation>().name()}, "" });
				}();

				updated_metas.metadatas.emplace_back(t_meta);
				generated_resources.emplace_back(t_guid, std::move(animation));
			}
		}
		// Here we decide whether or not to save as a prefab.
		// If importer_scene has no skeleton, then we only save as prefab if there is more than 1 mesh
		// If importer_scene has animation but no meshes, then we save the importer_scene as just an animation asset with no skeleton.
		// Check for errors here as well. Eg: Has skeleton but no mesh and no animation (Unlikely).

		// Check if this is a pure animation file. If it is, we don't even bother creating the prefab.
		// if (importer_scene.has_animation && !importer_scene.has_skeleton)
		// {
		// 	err_msg = "No mesh but animations were found. Saving as pure animation file.\n";
		// 	LOG_TO(LogPool::ANIM, err_msg);
		// 	return ret_val;
		// }
		// 
		// const auto scene = Core::GetSystem<SceneManager>().GetPrefabScene();
		// const auto prefab_root = scene->CreateGameObject();
		// prefab_root->Name(path_to_resource.GetStem());
		// if (importer_scene.has_meshes && importer_scene.has_skeleton)
		// {
		// 	for (auto& handle : mesh_handles)
		// 	{
		// 		const auto mesh_child = scene->CreateGameObject();
		// 		mesh_child->Name(handle->Name());
		// 		mesh_child->Transform()->SetParent(prefab_root);
		// 		const auto mesh_renderer = mesh_child->AddComponent<SkinnedMeshRenderer>();
		// 		mesh_renderer->mesh = RscHandle<Mesh>{ handle };
		// 	}
		// 	// Add an animator component to the prefab so that the skinned mesh renderer can draw.
		// 	// Need to revisit this as I feel this is the wrong way to do this.
		// 	const auto animator = prefab_root->AddComponent<Animator>();
		// 	animator->skeleton = skeleton_handle;
		// 	Core::GetSystem<AnimationSystem>().GenerateSkeletonTree(*animator);
		// 
		// 	for (auto& anim : animation_handles)
		// 		animator->AddAnimation(anim);
		// }
		// else if (importer_scene.has_meshes && !importer_scene.has_skeleton)
		// {
		// 	// If there is only 1 mesh, we attach it to the prefab itself. If not, we make child objects with MeshRenderer components.
		// 	if (importer_scene.num_meshes == 1)
		// 	{
		// 		// Just attach the mesh to the prefab itself
		// 		const auto mesh_renderer = prefab_root->AddComponent<MeshRenderer>();
		// 		mesh_renderer->mesh = mesh_handles[0];
		// 	}
		// 	else
		// 	{
		// 		for (auto& handle : mesh_handles)
		// 		{
		// 			const auto mesh_child = scene->CreateGameObject();
		// 			mesh_child->Name(handle->Name());
		// 			mesh_child->Transform()->SetParent(prefab_root);
		// 			const auto mesh_renderer = mesh_child->AddComponent<MeshRenderer>();
		// 			mesh_renderer->mesh = RscHandle<Mesh>{ handle };
		// 		}
		// 	}
		// }
		// 
		// {
		// 	auto prefab_meta = meta_bundle.FetchMeta<Prefab>();
		// 	const auto prefab_handle = prefab_meta
		// 		? PrefabUtility::Create(prefab_root, prefab_meta->guid)
		// 		: PrefabUtility::Create(prefab_root);
		// 	prefab_handle->Name(path_to_resource.GetStem());
		// 	ret_val.Add(prefab_handle);
		// 
		// 	LOG_TO(LogPool::ANIM, string{ "Saving meshes and animations as prefab.\n" });
		// }
		// scene->DestroyGameObject(prefab_root);
		return AssetBundle{ updated_metas, span<AssetBundle::AssetPair>{generated_resources} };
	}
}
