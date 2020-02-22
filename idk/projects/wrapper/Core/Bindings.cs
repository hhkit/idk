using System;
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

        /*
         * Scene
         */

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SceneChangeScene(Guid guid);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectNew();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void EngineKill();
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
        public extern static int GameObjectGetLayer(ulong gamehandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GameObjectSetLayer(ulong gamehandle, int layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectFindWithTag(string tag);

        /*
         * Component
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong ComponentGetGameObject(ulong componenthandle);

        /*
         * Behavior
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool MonoBehaviorGetEnable(ulong componenthandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void MonoBehaviorSetEnable(ulong componenthandle, bool enable);

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
         * BoxCollider
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderBoxSetCenter(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderBoxGetCenter(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderBoxSetSize(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderBoxGetSize(ulong id);
        /*
         * SphereCollider
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSphereSetCenter(ulong id, Vector3 val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 ColliderSphereGetCenter(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ColliderSphereSetRadius(ulong id, float val);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ColliderSphereGetRadius(ulong id);
        /*
         * Capsule Collider
         */

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
        /*
         * Animator
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorPlay(ulong id, string name, string layer = "");

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool AnimatorCrossFade(ulong id, string name, float time = 0.2f, string layer = "");

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
        /*
         * AudioSystem
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemSetVolume(float newVolume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void AudioSystemStopAll();



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
         * Graphics 
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsGetSRGB();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GraphicsSetSRGB(bool srgb);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GraphicsDisableGammaCorrection();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void GraphicsSetGammaCorrection(float gamma_correction);

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int CameraGetCullingMask(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CameraSetCullingMask(ulong id, int value);

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
        public extern static void ImageSetTexture(ulong id, Guid guid);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Guid ImageGetMaterialInstance(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ImageSetMaterialInstance(ulong id, Guid guid);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Color ImageGetColor(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ImageSetColor(ulong id, Color color);

        /*
         * Text
         */
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
        public extern static float InputGetAxis(sbyte index, int a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float InputSetRumble(sbyte index, float low_freq, float high_freq);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static byte InputGetConnectedPlayers();

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

        /*
         * LayerMask
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string LayerMaskLayerToName(int layerIndex);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int LayerMaskNameToLayer(string name);

        /*
         * Network
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool NetworkGetIsConnected();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool NetworkGetIsHost();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int NetworkGetPing();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkCreateLobby();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int[] NetworkGetPlayers();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkAddCallback(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkRemoveCallback(ulong handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Device[] NetworkGetDevices();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Address[] NetworkDeviceGetAddresses(string device);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkConnect(Address a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkDisconnect();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void NetworkLoadScene(Guid scene);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong NetworkInstantiatePrefabPosition(Guid guid, Vector3 pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong NetworkInstantiatePrefabPositionRotation(Guid guid, Vector3 pos, Quaternion rot);

        /*
         * ElectronView
         */
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
        public extern static int ViewDestroy(ulong handle);

    }
}
