﻿using System;
using System.IO;
using System.Xml.Serialization;

namespace idk
{
    public class Object
    {
        internal ulong handle;

        public static implicit operator bool(Object o)
        {
            if ((object) o == null)
                return false;

            return Bindings.ObjectValidate(o.handle);
        }

        public static bool operator == (Object lhs, Object rhs)
        {
            if (lhs && rhs)
                return lhs.handle == rhs.handle;
            else
                return lhs.Equals(null) && rhs.Equals(null);
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
                Object o = (Object)obj;
                return handle == o.handle;
            }
        }

        public override int GetHashCode()
        {
            return handle.GetHashCode() << 2;
        }

        // statics
        public static GameObject FindWithTag(string tag)
        {
            var id = Bindings.GameObjectFindWithTag(tag);
            return id != 0 ? new GameObject(id) : null;
        }

        public static void Destroy(Object o)
            => Bindings.ObjectDestroy(o.handle);

        public virtual string Serialize()
        {
            var type = GetType();
            var fields = type.GetFields();
            Console.WriteLine("Reflecting {0} fields", fields.Length);
            foreach (var field in fields)
            {
                Console.WriteLine("{0}: {1}", field.Name, field.GetValue(this));
            }
            return "";
        }

        public Object Clone()
        {
            return (Object) MemberwiseClone();
        }
    }
}
