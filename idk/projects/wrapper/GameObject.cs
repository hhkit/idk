using System;

namespace idk
{
    public class GameObject
        : IObject
    {
        internal ulong handle = 0;

        internal GameObject(ulong in_handle)
        {
            handle = in_handle;
        }

        public void PrintHandle(int i)
        {
            Console.WriteLine("handle: {0}", handle);
            Console.WriteLine("i : {0}", i);
        }
    }
}
