#pragma once

#include <ReflectReg_Common.h>
#include <prefab/PropertyOverride.h>
#include <IncludeResources.h>
#include <res/CompiledAssets.h>
#include <res/MetaBundle.h>

/*==========================================================================
 * general resources
 *========================================================================*/
#pragma region General Resources
	REFLECT_BEGIN(idk::SerializedMeta, "SerializedMeta")
	REFLECT_VARS(guid, name, t_hash, metadata)
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::SerializedMeta>, "vector<SerializedMeta>")
	REFLECT_END()

	REFLECT_BEGIN(idk::MetaBundle, "MetaBundle")
	REFLECT_VARS(metadatas)
	REFLECT_END()
#pragma endregion
	/*==========================================================================
	 * specific resources
	 *========================================================================*/
#pragma region Specific Resources

	REFLECT_ENUM(idk::ColorFormat, "ColorFormat")
	REFLECT_ENUM(idk::UVMode, "UVMode")
	REFLECT_ENUM(idk::FilterMode, "FilterMode")
	REFLECT_ENUM(idk::AntiAliasing, "AntiAliasing")

	REFLECT_BEGIN(idk::Texture::Metadata, "TextureMeta")
	REFLECT_VARS(uv_mode, internal_format, filter_mode, is_srgb)
	REFLECT_END()

	REFLECT_BEGIN(idk::RenderTarget, "RenderTarget")
	REFLECT_VARS(size, anti_aliasing, is_world_renderer, render_debug, color_tex, depth_tex)
	REFLECT_END()

	//REFLECT_ENUM(idk::CMColorFormat, "CMColorFormat")
	//REFLECT_ENUM(idk::CMUVMode, "CMUVMode")

	REFLECT_BEGIN(idk::CubeMap::Metadata, "CubeMapMeta")
	REFLECT_VARS(uv_mode, internal_format)
	REFLECT_END()

	//REFLECT_ENUM(idk::FontColorFormat, "FontColorFormat")
	//REFLECT_ENUM(idk::FontUVMode, "FontUVMode")

	REFLECT_BEGIN(idk::FontAtlas::Metadata, "FontAtlasMeta")
	REFLECT_VARS(uv_mode, internal_format)
	REFLECT_END()

	REFLECT_BEGIN(idk::TestResource, "TestResource")
	REFLECT_VARS(k, yolo)
	REFLECT_END()

	REFLECT_BEGIN(decltype(idk::Material::uniforms), "hash_table<string,UniformInstance>")
	REFLECT_END()

	REFLECT_BEGIN(decltype(idk::MaterialInstance::uniforms), "hash_table<string,UniformInstanceValue>")
	REFLECT_END()

	REFLECT_ENUM(idk::BlendMode, "BlendMode")
	REFLECT_ENUM(idk::MaterialDomain, "MaterialDomain")
	REFLECT_ENUM(idk::ShadingModel, "ShadingModel")


	REFLECT_BEGIN(idk::Material, "Material")
	REFLECT_VARS(_shader_program, uniforms, domain, blend, model)
	REFLECT_END()

	REFLECT_BEGIN(idk::UniformInstance, "UniformInstance")
	REFLECT_VARS(name, value)
	REFLECT_END()
	REFLECT_BEGIN(idk::MaterialInstance, "MaterialInstance")
	REFLECT_VARS(material, uniforms)
	REFLECT_END()

	REFLECT_BEGIN(idk::PrefabData, "PrefabData")
	REFLECT_VARS(parent_index, components)
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::PrefabData>, "vector<PrefabData>")
	REFLECT_END()

	REFLECT_BEGIN(idk::Prefab, "Prefab")
	REFLECT_VARS(data)
	REFLECT_END()

	REFLECT_BEGIN(idk::PropertyOverride, "PropertyOverride")
	REFLECT_VARS(component_name, property_path, component_nth)
	REFLECT_END()


	REFLECT_BEGIN(idk::anim::KeyFrame<idk::vec3>, "KeyFrame<vec3>")
	REFLECT_VARS(val, time)
	REFLECT_END()

	REFLECT_BEGIN(idk::anim::KeyFrame<idk::quat>, "KeyFrame<quat>")
	REFLECT_VARS(val, time)
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::anim::KeyFrame<idk::vec3>>, "vector<KeyFrame<vec3>>")
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::anim::KeyFrame<idk::quat>>, "vector<KeyFrame<quat>>")
	REFLECT_END()

	REFLECT_BEGIN(idk::anim::AnimatedBone, "AnimatedBone")
	REFLECT_VARS(bone_name, translate_track, scale_track, rotation_track)
	REFLECT_END()

	REFLECT_BEGIN(decltype(idk::anim::Animation::_animated_bones), "hash_table<string,AnimatedBone>")
	REFLECT_END()

	REFLECT_BEGIN(idk::anim::Animation, "Animation")
	REFLECT_VARS(_animated_bones, _fps, _duration, _num_ticks)
	REFLECT_END()

	REFLECT_BEGIN(decltype(idk::anim::Skeleton::_bone_table), "hash_table<string,size_t>")
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<int>, "vector<int>")
	REFLECT_END()

	REFLECT_BEGIN(idk::anim::BoneData, "BoneData")
	REFLECT_VARS(name, parent, children, global_inverse_bind_pose, pre_rotation, post_rotation, local_bind_pose)
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::anim::BoneData>, "vector<BoneData>")
	REFLECT_END()

	REFLECT_BEGIN(idk::anim::Skeleton, "idk::anim::Skeleton")
	REFLECT_VARS(_global_inverse, _bone_table, _bones)
	REFLECT_END()

	REFLECT_ENUM(idk::vtx::Attrib, "Attribute")

	REFLECT_BEGIN(idk::vtx::Descriptor, "Descriptor")
	REFLECT_VARS(attrib, stride, offset)
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::vtx::Descriptor>, "vector<Descriptor>")
	REFLECT_END()

	REFLECT_BEGIN(idk::CompiledBuffer, "CompiledBuffer")
	REFLECT_VARS(attribs, data)
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::CompiledBuffer>, "vector<CompiledBuffer>")
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<unsigned>, "vector<unsigned>")
	REFLECT_END()

	REFLECT_BEGIN(idk::CompiledMesh, "CompiledMesh")
	REFLECT_VARS(buffers, element_buffer, bounding_volume)
	REFLECT_END()

	// shader graph
	REFLECT_ENUM(idk::shadergraph::ValueType, "ShaderGraphValueType")

	REFLECT_BEGIN(idk::shadergraph::Link, "ShaderGraphLink")
	REFLECT_VARS(node_out, node_in, slot_out, slot_in)
	REFLECT_END()

	REFLECT_BEGIN(idk::shadergraph::Slot, "ShaderGraphSlot")
	REFLECT_VARS(type, value)
	REFLECT_END()

	REFLECT_BEGIN(idk::shadergraph::Parameter, "ShaderGraphParameter")
	REFLECT_VARS(name, type, default_value)
	REFLECT_END()

	REFLECT_BEGIN(decltype(idk::shadergraph::Graph::nodes), "hash_table<Guid,ShaderGraphNode>")
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::shadergraph::Slot>, "vector<ShaderGraphSlot>")
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::shadergraph::Link>, "vector<ShaderGraphLink>")
	REFLECT_END()

	REFLECT_BEGIN(idk::vector<idk::shadergraph::Parameter>, "vector<ShaderGraphParameter")
	REFLECT_END()

	REFLECT_BEGIN(idk::shadergraph::Node, "ShaderGraphNode")
	REFLECT_VARS(name, guid, position, input_slots, output_slots, control_values)
	REFLECT_END()

	REFLECT_BEGIN(idk::shadergraph::Graph, "ShaderGraph")
	REFLECT_VARS(_shader_program, master_node, nodes, links, parameters)
	REFLECT_END()

#pragma endregion