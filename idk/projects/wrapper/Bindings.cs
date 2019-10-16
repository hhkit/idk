using System;
using System.Runtime.CompilerServices;

namespace idk
{
    internal class Bindings
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GameObjectGetActiveInHierarchy(ulong gamehandle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool GameObjectActiveSelf(ulong gamehandle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GameObjectSetActive(ulong gamehandle, bool active);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectAddEngineComponent(ulong gamehandle, string component);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong GameObjectGetEngineComponent(ulong gamehandle, string component);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetPosition(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformSetPosition(ulong id, Vector3 val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetScale(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformSetScale(ulong id, Vector3 val);
    }
}
