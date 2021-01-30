using System;
using System.Runtime.CompilerServices;

namespace idk
{
    internal class Bindings
    {

        #region Object

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ObjectValidate(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ObjectDestroy(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static MonoBehavior[] ObjectGetObjectsOfType(string type);

        #endregion

        #region Scene

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SceneChangeScene(Guid guid);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid SceneGetActiveScene();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectNew();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void EngineKill();

        #endregion

        #region Game Object

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
        public extern static int GameObjectGetLayer(ulong gamehandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GameObjectSetLayer(ulong gamehandle, int layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectFindWithTag(string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static GameObject[] GameObjectFindGameObjectsWithTag(string tag);

        #endregion

        #region Component

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong ComponentGetGameObject(ulong componenthandle);

        #endregion

        #region Behavior

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool MonoBehaviorGetEnable(ulong componenthandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MonoBehaviorSetEnable(ulong componenthandle, bool enable);

        #endregion

        #region Transform

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

        #endregion

        #region Physics

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ManagedRaycast PhysicsRaycast(Vector3 origin, Vector3 dir, float max_dist, int mask, bool hit_triggers);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ManagedRaycast[] PhysicsRaycastAll(Vector3 origin, Vector3 dir, float max_dist, int mask, bool hit_triggers);

        #endregion

        #region RigidBody

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
        public extern static float RigidBodyGetGravityScale(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySetGravityScale(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodySleep(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodyTeleport(ulong id, Vector3 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RigidBodyAddForce(ulong id, Vector3 force);
        
        #endregion

        #region Collider

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetEnabled(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ColliderGetEnabled(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetTrigger(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ColliderGetTrigger(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetStaticFriction(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderGetStaticFriction(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetDynamicFriction(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderGetDynamicFriction(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSetBounciness(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderGetBounciness(ulong id);

        #endregion

        #region BoxCollider

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderBoxSetCenter(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderBoxGetCenter(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderBoxSetSize(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderBoxGetSize(ulong id);

        #endregion

        #region SphereCollider

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSphereSetCenter(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderSphereGetCenter(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSphereSetRadius(ulong id, float val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderSphereGetRadius(ulong id);

        #endregion

        #region Capsule Collider

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderCapsuleSetCenter(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderCapsuleGetCenter(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderCapsuleSetDirection(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderCapsuleGetDirection(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderCapsuleSetRadius(ulong id, float val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderCapsuleGetRadius(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderCapsuleSetHeight(ulong id, float val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderCapsuleGetHeight(ulong id);

        #endregion

        #region Animator

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorPlay(ulong id, string name, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorBlendTo(ulong id, string name, float time = 0.2f, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorBlendToInSeconds(ulong id, string name, float timeInSeconds = 0.0f, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorPause(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorResume(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorStop(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static AnimationState AnimatorGetState(ulong id, string name, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AnimatorDefaultStateName(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AnimatorCurrentStateName(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AnimatorCurrentStateTime(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AnimatorBlendStateName(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorIsPlaying(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorIsBlending(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorHasCurrAnimEnded(ulong id, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorHasState(ulong id, string name, string layer = "");

        // Parameter Getters
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int AnimatorGetInt(ulong id, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AnimatorGetFloat(ulong id, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorGetBool(ulong id, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorGetTrigger(ulong id, string name);

        // Parameter Setters
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorSetInt(ulong id, string name, int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorSetFloat(ulong id, string name, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorSetBool(ulong id, string name, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorSetTrigger(ulong id, string name, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AnimatorGetWeight(ulong id, string name = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorSetWeight(ulong id, float weight, string name = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 AnimatorGetOffset(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AnimatorSetOffset(ulong handle, Vector3 value);

        #endregion

        #region AudioListener

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioListenerGetEnabledState(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioListenerSetEnabledState(ulong id, bool c);


        #endregion

        #region AudioSource

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
        public extern static float AudioSourceClipGetVolume(ulong id, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSourceClipSetVolume(ulong id, int index, float volume);

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

        #endregion

        #region AudioSystem

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetMASTERVolume(float newVolume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSystemGetMASTERVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetSFXVolume(float newVolume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSystemGetSFXVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetMUSICVolume(float newVolume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSystemGetMUSICVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetAMBIENTVolume(float newVolume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSystemGetAMBIENTVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetDIALOGUEVolume(float newVolume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float AudioSystemGetDIALOGUEVolume();


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetMASTERPause(bool newState);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSystemGetMASTERPause();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetSFXPause(bool newState);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSystemGetSFXPause();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetMUSICPause(bool newState);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSystemGetMUSICPause();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetAMBIENTPause(bool newState);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSystemGetAMBIENTPause();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetDIALOGUEPause(bool newState);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AudioSystemGetDIALOGUEPause();


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemStopAll();

        #endregion

        #region TextMesh

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string TextMeshGetText(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextMeshSetText(ulong id, string c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color TextMeshGetColor(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextMeshSetColor(ulong id, Color c);

        #endregion

        #region Graphics 

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsGetSRGB();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GraphicsSetSRGB(bool srgb);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsDisableGammaCorrection();
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float GraphicsGetGammaCorrection();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GraphicsSetGammaCorrection(float gamma_correction);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GraphicsSetVarBool(string var_name, bool val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsGetVarBool(string var_name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsVarBoolIsSet(string var_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GraphicsSetVarInt(string var_name, int val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int GraphicsGetVarInt(string var_name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsVarIntIsSet(string var_name);

        #endregion

        #region Camera

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetFOV(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetFOV(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetNearPlane(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetNearPlane(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetFarPlane(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetFarPlane(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Rect CameraGetViewport(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetViewport(ulong id, Rect value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool CameraGetEnabledState(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetEnabledState(ulong id, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int CameraGetCullingMask(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetCullingMask(ulong id, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int CameraGetUseFog(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetUseFog(ulong id, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int CameraGetUseBloom(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetUseBloom(ulong id, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color CameraGetFogColor(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetFogColor(ulong id, Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetFogDensity(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetFogDensity(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 CameraGetThreshold(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetThreshold(ulong id, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetBlurStrength(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetBlurStrength(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float CameraGetBlurScale(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetBlurScale(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetDepth(ulong handle, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int CameraGetDepth(ulong handle);

        #endregion

        #region Light

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool LightGetEnabled(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetEnabled(ulong id, bool value);

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
        public extern static float LightGetAttenuationRadius(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetAttenuationRadius(ulong id, float i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool LightGetIsInverseSqAtt(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetIsInverseSqAtt(ulong id, bool i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float LightGetInnerSpotAngle(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetInnerSpotAngle(ulong id, float i);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float LightGetOuterSpotAngle(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LightSetOuterSpotAngle(ulong id, float i);

        #endregion

        #region Renderer

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid RendererGetMaterialInstance(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RendererSetMaterialInstance(ulong handle, Guid guid);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RendererGetActive(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RendererSetActive(ulong id, bool active);

        #endregion

        #region RectTransform

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetOffsetMin(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectTransformSetOffsetMin(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetOffsetMax(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectTransformSetOffsetMax(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetAnchorMin(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectTransformSetAnchorMin(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetAnchorMax(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectTransformSetAnchorMax(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetPivot(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectTransformSetPivot(ulong id, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Rect RectTransformGetRect(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 RectTransformGetAnchoredPosition(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectTransformSetAnchoredPosition(ulong id, Vector2 value);

        #endregion

        #region Image

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid ImageGetTexture(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ImageSetTexture(ulong id, Guid guid);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid ImageGetMaterialInstance(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ImageSetMaterialInstance(ulong id, Guid guid);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color ImageGetColor(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ImageSetColor(ulong id, Color color);

        #endregion

        #region Text

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string TextGetText(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextSetText(ulong id, string c);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid TextGetMaterialInstance(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextSetMaterialInstance(ulong id, Guid guid);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color TextGetColor(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextSetColor(ulong id, Color color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static uint TextGetFontSize(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TextSetFontSize(ulong id, uint value);

        #endregion

        #region Resource

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ResourceValidate(System.Guid guid, string type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string ResourceGetName(System.Guid guid, string type);

        #endregion

        #region Prefab

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong PrefabInstantiate(System.Guid guid);

        #endregion

        #region MaterialInstance

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

        #endregion

        #region Texture

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector2 TextureGetSize(System.Guid guid);

        #endregion

        #region Input

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetKey(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetKeyDown(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetKeyUp(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float InputGetAxis(sbyte index, int a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetAnyKey();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InputGetAnyKeyDown();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float InputSetRumble(sbyte index, float low_freq, float high_freq);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static byte InputGetConnectedPlayers();

        #endregion

        #region Time

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetTimeSinceStart();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetAccumTime();

        #endregion

        #region Debug

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float DebugLog(string preface, string message);

        #endregion

        #region LayerMask

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string LayerMaskLayerToName(int layerIndex);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int LayerMaskNameToLayer(string name);

        #endregion

        #region Network

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float NetworkGetPredictionWeight();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkSetPredictionWeight(float new_weight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool NetworkGetIsConnected();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool NetworkGetIsRollingBack();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool NetworkGetIsHost();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int NetworkGetPing();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkCreateLobby(int lobbyType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkJoinLobby(ulong lobby);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkLeaveLobby();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong NetworkGetCurrentLobby();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int NetworkGetLocalClient();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkOpenLobbyInviteDialog();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkFindLobbies();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string NetworkGetLobbyData(ulong lobby, string key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkSetLobbyData(string key, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkSendLobbyMsg(byte[] msg);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int NetworkGetLobbyNumMembers(ulong lobby);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int NetworkClientLobbyIndex(int id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string NetworkClientName(int id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkConnectToLobbyOwner();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int[] NetworkGetClients();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkAddCallback(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkRemoveCallback(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkDisconnect();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkLoadScene(Guid scene);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong NetworkInstantiatePrefabPosition(Guid guid, Vector3 pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong NetworkInstantiatePrefabPositionRotation(Guid guid, Vector3 pos, Quaternion rot);

        #endregion

        #region ElectronView

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static uint ViewGetNetworkId(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ViewIsMine(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ViewTransferOwnership(ulong handle, int newID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int ViewExecRPC(ulong handle, string method, RPCTarget target, byte[][] parameters);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int ViewExecRPCOnPlayer(ulong handle, string method, int target, byte[][] parameters);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong ViewIdGetView(uint id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ViewDestroy(ulong handle);

        #endregion

    }
}
