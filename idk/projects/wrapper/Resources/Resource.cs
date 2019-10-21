namespace idk
{
    public class Resource
    {
        internal System.Guid guid;

        public string name { get => Bindings.ResourceGetName(guid, GetType().Name); }

        public static implicit operator bool(Resource o)
        {
            return o == null ? false : Bindings.ResourceValidate(o.guid, o.GetType().Name);
        }

        public static bool operator ==(Resource lhs, Resource rhs)
        {
            if (lhs.GetType() != rhs.GetType())
                return false;
            else if (lhs && rhs)
                return lhs.guid == rhs.guid;
            else
                return lhs == null && rhs == null;
        }

        public static bool operator !=(Resource lhs, Resource rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object obj)
        {
            if ((obj == null) || !GetType().Equals(obj.GetType()))
            {
                return false;
            }
            else
            {
                Resource o = (Resource)obj;
                return guid == o.guid;
            }
        }

        public override int GetHashCode()
        {
            return guid.GetHashCode();
        }
    }
}
