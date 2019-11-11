namespace idk
{
    public class Resource
    {
        internal System.Guid guid;

        public string name { get => Bindings.ResourceGetName(guid, GetType().Name); }

        public static implicit operator bool(Resource o)
        {
            return (object) o == null ? false : Bindings.ResourceValidate(o.guid, o.GetType().Name);
        }

        public static bool operator ==(Resource lhs, Resource rhs)
        {
            if (lhs && rhs)
            {
                if (lhs.GetType() != rhs.GetType())
                    return false;
                else
                    return lhs.guid == rhs.guid;
            }
            else
                return (object)lhs == null && (object)rhs == null;
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

        public override string ToString()
        {
            return guid.ToString();
        }
    }
}
