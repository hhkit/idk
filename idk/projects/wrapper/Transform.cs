using System;
using System.Runtime.CompilerServices;

namespace idk
{
    public class Transform
        : Component
    {
        public Vector3 position {
            get => Bindings.TransformGetPosition(handle);
            set => Bindings.TransformSetPosition(handle, value);
        }
        public Transform()
        {
            Console.WriteLine("tfm ctor");
        }
    }
}
