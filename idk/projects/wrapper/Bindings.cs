using System;
using System.Runtime.CompilerServices;

namespace idk
{
    internal class Bindings
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Vector3 TransformGetPosition(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TransformSetPosition(ulong id, Vector3 val);
    }
}
