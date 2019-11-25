﻿using System;
using System.Runtime.CompilerServices;

namespace idk
{
    internal class Bindings
    {
        /*
         * Object
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ObjectValidate(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ObjectDestroy(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static MonoBehavior[] ObjectGetObjectsOfType(string type);


        /**
         * Game Object
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GameObjectGetActiveInHierarchy(ulong gamehandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GameObjectActiveSelf(ulong gamehandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GameObjectSetActive(ulong gamehandle, bool active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectAddEngineComponent(ulong gamehandle, string component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectGetEngineComponent(ulong gamehandle, string component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static MonoBehavior GameObjectAddGameComponent(ulong gamehandle, string component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static MonoBehavior GameObjectGetGameComponent(ulong gamehandle, string component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GameObjectGetName(ulong gamehandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GameObjectSetName(ulong gamehandle, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GameObjectGetTag(ulong gamehandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GameObjectSetTag(ulong gamehandle, string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectFindWithTag(string tag);

        /*
         * Component
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong ComponentGetGameObject(ulong componenthandle);

        /*
         * Transform
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetPosition(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetPosition(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetScale(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetScale(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Quaternion TransformGetRotation(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetRotation(ulong id, Quaternion val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetLocalPosition(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetLocalPosition(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetLocalScale(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetLocalScale(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Quaternion TransformGetLocalRotation(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetLocalRotation(ulong id, Quaternion val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformForward(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformUp(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformRight(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong TransformGetParent(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetParent(ulong id, ulong parent, bool preserve_global);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static GameObject[] TransformGetChildren(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetForward(ulong id, Vector3 fwd);

        /*
         * Physics
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ManagedRaycast PhysicsRaycast(Vector3 origin, Vector3 dir, float max_dist, int mask, bool hit_triggers);

        /*
         * RigidBody
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float RigidBodyGetMass(ulong id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySetMass(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 RigidBodyGetVelocity(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySetVelocity(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 RigidBodyGetPosition(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySetPosition(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RigidBodyGetIsKinematic(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySetIsKinematic(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RigidBodyGetUseGravity(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySetUseGravity(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySleep(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodyTeleport(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodyAddForce(ulong id, Vector3 force);

        /*
         * Animator
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AnimatorPlay(ulong id, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AnimatorCrossFade(ulong id, string name, float time = 0.2f);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AnimatorPause(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AnimatorResume(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AnimatorStop(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AnimatorDefaultStateName(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AnimatorCurrentStateName(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AnimatorBlendStateName(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorIsPlaying(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorIsBlending(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorHasCurrAnimEnded(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorHasState(ulong id, string name);

        /*
         * AudioSource
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourcePlay(ulong id, int index = 0);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourcePlayAll(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourceStop(ulong id, int index = 0);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourceStopAll(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSourceGetVolume(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourceSetVolume(ulong id, float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSourceGetPitch(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourceSetPitch(ulong id, float pitch);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSourceGetLoop(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourceSetLoop(ulong id, bool loop);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int AudioSourceSize(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSourceIsAudioClipPlaying(ulong id, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSourceIsAnyAudioClipPlaying(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int FindAudio(ulong id, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int AddAudioClip(ulong id, string name);

        /*
        * TextMesh
        */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string TextMeshGetText(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextMeshSetText(ulong id, string c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color TextMeshGetColor(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextMeshSetColor(ulong id, Color c);

        /*
        * Camera
        */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Rad CameraGetFOV(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetFOV(ulong id, Rad c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetNearPlane(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetNearPlane(ulong id, float c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetFarPlane(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetFarPlane(ulong id, float c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Rect CameraGetViewport(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetViewport(ulong id, Rect c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool CameraGetEnabledState(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetEnabledState(ulong id, bool c);

        /*
        * Light
        */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color LightGetColor(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetColor(ulong id, Color c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float LightGetIntensity(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetIntensity(ulong id, float i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool LightGetCastShadow(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetCastShadow(ulong id, bool i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float LightGetShadowBias(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetShadowBias(ulong id, float i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Rad LightGetFOV(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetFOV(ulong id, Rad i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float LightGetAttenuationRadius(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetAttenuationRadius(ulong id, float i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool LightGetIsInverseSqAtt(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetIsInverseSqAtt(ulong id, bool i);

        /*
         * Collider
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetEnabled(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ColliderGetEnabled(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetTrigger(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ColliderGetTrigger(ulong id);

        /*
         * Renderer
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid RendererGetMaterialInstance(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RendererSetMaterialInstance(ulong handle, Guid guid);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RendererGetActive(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RendererSetActive(ulong id, bool active);

        /*
         * RectTransform
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetOffsetMin(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformSetOffsetMin(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetOffsetMax(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformSetOffsetMax(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetAnchorMin(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformSetAnchorMin(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetAnchorMax(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformSetAnchorMax(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetPivot(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformSetPivot(ulong id, Vector2 value);

        /*
         * Image
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid ImageGetTexture(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid ImageGetMaterialInstance(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color ImageGetColor(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ImageSetColor(ulong id, Color color);

        /*
         * Resource
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ResourceValidate(System.Guid guid, string type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string ResourceGetName(System.Guid guid, string type);

        /*
         * Prefab
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong PrefabInstantiate(System.Guid guid);

        /*
         * MaterialInstance
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float MaterialInstanceGetFloat(System.Guid guid, string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 MaterialInstanceGetVector2(System.Guid guid, string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 MaterialInstanceGetVector3(System.Guid guid, string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector4 MaterialInstanceGetVector4(System.Guid guid, string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid MaterialInstanceGetTexture(System.Guid guid, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MaterialInstanceSetFloat(System.Guid guid, string name, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MaterialInstanceSetVector2(System.Guid guid, string name, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MaterialInstanceSetVector3(System.Guid guid, string name, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MaterialInstanceSetVector4(System.Guid guid, string name, Vector4 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MaterialInstanceSetTexture(System.Guid guid, string name, Guid tex);

        /*
         * Input
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetKey(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetKeyDown(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetKeyUp(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float InputGetAxis(char index, int a);

        /*
         * Time
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetTimeScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TimeSetTimeScale(float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetFixedDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetUnscaledFixedDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetUnscaledDelta();

        /*
         * Debug
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float DebugLog(string preface, string message);
    }
}
