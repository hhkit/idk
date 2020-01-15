#pragma once

#include <ReflectReg_Common.inl>
#include <IncludeSystems.h>


/*==========================================================================
 * configs
 *========================================================================*/
#pragma region Configs
REFLECT_BEGIN(idk::TagManager, "TagManager")
REFLECT_END()

REFLECT_BEGIN(idk::TagManagerConfig, "TagManagerConfig")
REFLECT_VARS(tags)
REFLECT_END()

REFLECT_BEGIN(idk::LayerManager, "LayerManager")
REFLECT_END()

REFLECT_BEGIN(decltype(idk::LayerManagerConfig::layers), "array<string,32>")
REFLECT_END()

REFLECT_BEGIN(idk::LayerManagerConfig, "LayerManagerConfig")
REFLECT_VARS(layers)
REFLECT_END()

REFLECT_BEGIN(idk::SceneManager, "SceneManager")
REFLECT_VARS(_startup_scene)
REFLECT_END()

REFLECT_BEGIN(idk::mono::ScriptSystemConfig, "ScriptConfig")
REFLECT_VARS(path_to_game_dll)
REFLECT_END()

REFLECT_BEGIN(idk::mono::ScriptSystem, "ScriptSystem")
REFLECT_END()

REFLECT_BEGIN(decltype(idk::PhysicsConfig::matrix), "PhysicsLayerMatrix")
REFLECT_END()

REFLECT_BEGIN(idk::PhysicsConfig, "PhysicsConfig")
REFLECT_VARS(matrix, batch_size)
REFLECT_END()

REFLECT_BEGIN(idk::PhysicsSystem, "PhysicsSystem")
REFLECT_END()
#pragma endregion

