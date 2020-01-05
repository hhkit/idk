#pragma once
#include <ReflectReg_Common.inl>
#include <IncludeComponents.h>

// ANIMATION
#pragma region Animation
#pragma region Enums
// Enums
REFLECT_ENUM(idk::AnimLayerBlend, "AnimLayerBlend")
REFLECT_ENUM(idk::anim::AnimDataType, "AnimDataType")
REFLECT_ENUM(idk::anim::ConditionIndex, "ConditionIndex")
REFLECT_ENUM(idk::anim::BlendTreeType, "BlendTreeType")
#pragma endregion

// Animation States reflection
#pragma region Animation States Reflection
REFLECT_BEGIN(idk::BasicAnimationState, "BasicAnimationState")
REFLECT_VARS(motion)
REFLECT_END()

REFLECT_BEGIN(decltype(idk::BlendTreeMotion::thresholds), "array<float,2>")
REFLECT_END();

REFLECT_BEGIN(idk::BlendTreeMotion, "BlendTreeMotion")
REFLECT_VARS(motion, thresholds, speed)
REFLECT_END()

REFLECT_BEGIN(decltype(idk::BlendTree::motions), "vector<BlendTreeMotion>")
REFLECT_END();

REFLECT_BEGIN(decltype(idk::BlendTree::params), "array<string,2>")
REFLECT_END();

REFLECT_BEGIN(idk::BlendTree, "BlendTree")
REFLECT_VARS(motions, params, blend_tree_type)
REFLECT_END()
#pragma endregion
#pragma region Animation Condition and Transitions
// Animation conditions and transitions
REFLECT_BEGIN(idk::AnimationCondition, "AnimationCondition")
REFLECT_VARS(param_name, type, op_index, val_f, val_i, val_b, val_t)
REFLECT_END()

REFLECT_BEGIN(decltype(idk::AnimationTransition::conditions), "vector<AnimationCondition>")
REFLECT_END();

REFLECT_BEGIN(idk::AnimationTransition, "AnimationTransition")
REFLECT_VARS(valid, transition_from_index, transition_to_index, interruptible, has_exit_time, exit_time, transition_offset, transition_duration, conditions)
REFLECT_END()

REFLECT_BEGIN(decltype(idk::AnimationState::transitions), "vector<AnimationTransition>")
REFLECT_END();
REFLECT_BEGIN(decltype(idk::AnimationState::state_data), "variant<BasicAnimationState,BlendTree>")
REFLECT_END();

REFLECT_BEGIN(idk::AnimationState, "AnimationState")
REFLECT_VARS(name, valid, loop, speed, state_data, node_position, transitions)
REFLECT_END()
#pragma endregion
// Animation Layer serialization
#pragma region Animation Layer Serialization
REFLECT_BEGIN(decltype(idk::AnimationLayer::anim_states), "vector<AnimationState>")
REFLECT_END()

REFLECT_BEGIN(idk::AnimationLayer, "AnimationLayer")
REFLECT_VARS(name, default_index, default_weight, anim_states, bone_mask, blend_type)
REFLECT_END()
#pragma endregion
// Animation parameters
#pragma region Animation Parameters
REFLECT_BEGIN(idk::anim::IntParam, "anim::IntParam")
REFLECT_VARS(name, def_val, val, valid)
REFLECT_END()

REFLECT_BEGIN(idk::anim::FloatParam, "anim::FloatParam")
REFLECT_VARS(name, def_val, val, valid)
REFLECT_END()

REFLECT_BEGIN(idk::anim::BoolParam, "anim::BoolParam")
REFLECT_VARS(name, def_val, val, valid)
REFLECT_END()

REFLECT_BEGIN(idk::anim::TriggerParam, "anim::TriggerParam")
REFLECT_VARS(name, def_val, val, valid)
REFLECT_END()

REFLECT_BEGIN(decltype(idk::Animator::AnimationParams::int_vars), "hash_table<string,anim::IntParam>")
REFLECT_END()

REFLECT_BEGIN(decltype(idk::Animator::AnimationParams::float_vars), "hash_table<string,anim::FloatParam>")
REFLECT_END()

REFLECT_BEGIN(decltype(idk::Animator::AnimationParams::bool_vars), "hash_table<string,anim::BoolParam>")
REFLECT_END()

REFLECT_BEGIN(decltype(idk::Animator::AnimationParams::trigger_vars), "hash_table<string,anim::TriggerParam>")
REFLECT_END()

REFLECT_BEGIN(idk::Animator::AnimationParams, "parameters")
REFLECT_VARS(int_vars, float_vars, bool_vars, trigger_vars)
REFLECT_END()
#pragma endregion

// Animator serialization
#pragma region Animator Serialization
REFLECT_BEGIN(idk::vector<idk::AnimationLayer>, "vector<AnimationLayer>")
REFLECT_END()

REFLECT_BEGIN(idk::Animator, "Animator")
REFLECT_VARS(skeleton, layers, parameters)
REFLECT_END()

REFLECT_BEGIN(idk::Bone, "Bone")
REFLECT_VARS(bone_name, bone_index)
REFLECT_END()
#pragma endregion

#pragma endregion