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
        public extern static Vector3 TransformForward(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformUp(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformRight(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong TransformGetParent(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void TransformSetParent(ulong id, ulong parent, bool preserve_global);

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
        public extern static float TimeGetFixedDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float TimeGetDelta();

        /*
         * Debug
         */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float DebugLog(string preface, string message);

    }
}
