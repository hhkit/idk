using System;

namespace idk
{
    public class Object
    {
        internal ulong handle;

        public static implicit operator bool(Object o)
        {
            return o == null ? false : Bindings.ObjectValidate(o.handle);
        }

        public static bool operator == (Object lhs, Object rhs)
        {
            if (lhs && rhs)
                return lhs.handle == rhs.handle;
            else
                return lhs == null && rhs == null;
        }

        public static bool operator !=(Object lhs, Object rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object obj) 
        {
            if ((obj == null) || !this.GetType().Equals(obj.GetType()))
            {
                return false;
            }
            else
            {
                Object o = (Object )obj;
                return handle == o.handle;
            }
        }

        public override int GetHashCode()
        {
            return handle.GetHashCode() << 2;
        }

        public static void Destroy(Object o)
            => Bindings.ObjectDestroy(o.handle);
    }
}
