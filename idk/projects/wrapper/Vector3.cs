using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float x;
        public float y;
        public float z;

        // constructors
        public Vector3(Vector3 v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        public Vector3(float in_x, float in_y, float in_z)
        {
            x = in_x;
            y = in_y;
            z = in_z;
        }

        // properties
        public float sqrMagnitude
        { get { return x * x + y * y + z * z; } }
        public float magnitude
        { get { return Mathf.Sqrt(sqrMagnitude);  } }
        public Vector3 normalized
        { get { return this / magnitude; } }
        
        // member functions
        public bool Equals(Vector3 rhs)
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
        public Vector3 Normalize()
        {
            var mag = magnitude;
            x /= mag;
            y /= mag;
            z /= mag;
            return this;
        }
        public void Set(float newX, float newY)
        {
            x = newX;
            y = newY;
        }

        // static methods
        public static Vector3 ClampMagnitude(Vector3 v, float maxLength)
        {
            if (v.magnitude > maxLength)
                return v.normalized * maxLength;
            else
                return v;
        }
        public static double Distance(Vector3 lhs, Vector3 rhs)
        {
            return (lhs-rhs).magnitude;
        }
        public static double Dot(Vector3 lhs, Vector3 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }
        public static Vector3 Lerp(Vector3 lhs, Vector3 rhs, float t)
        {
            return LerpUnclamped(lhs, rhs, t < 0 ? 0 : t > 1 ? 1 : t);
        }
        public static Vector3 LerpUnclamped(Vector3 lhs, Vector3 rhs, float t)
        {
            return (1-t) * lhs + (t) * rhs;
        }
        public static Vector3 Scale(Vector3 lhs, Vector3 rhs)
        {
            return new Vector3(lhs.x * rhs.x, rhs.y * rhs.y, lhs.z * rhs.z);
        }

        public override bool Equals(object obj)
        {
            return this == (Vector3)obj;
        }
        public override int GetHashCode()
        {
            return (x.GetHashCode() ^ y.GetHashCode() << 2) ^ z.GetHashCode();
        }

        // operator overloads
        public float this[int key]
        {
            get
            {
                switch(key)
                {
                    case 0: return x;
                    case 1: return y;
                    case 2: return z;
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
            set
            {
                switch (key)
                {
                    case 0: x= value; break;
                    case 1: y= value; break;
                    case 2: z= value; break;
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
        }
        public static bool operator ==(Vector3 lhs, Vector3 rhs)
        {
            return Mathf.Abs(lhs.x - rhs.x) < Mathf.Epsilon
                && Mathf.Abs(lhs.y - rhs.y) < Mathf.Epsilon
                && Mathf.Abs(lhs.z - rhs.z) < Mathf.Epsilon;
        }
        public static bool operator !=(Vector3 lhs, Vector3 rhs)
        {
            return !(lhs == rhs);
        }
        public static Vector3 operator + (Vector3 lhs, Vector3 rhs)
        {
            var returnme = new Vector3(lhs);
            returnme.x += rhs.x;
            returnme.y += rhs.y;
            returnme.z += rhs.z;
            return returnme;
        }
        public static Vector3 operator - (Vector3 lhs, Vector3 rhs)
        {
            var returnme = new Vector3(lhs);
            returnme.x -= rhs.x;
            returnme.y -= rhs.y;
            returnme.z -= rhs.z;
            return returnme;
        }
        public static Vector3 operator - (Vector3 lhs)
        {
            var returnme = new Vector3(lhs);
            return returnme *= -1;
        }
        public static Vector3 operator * (Vector3 lhs, float rhs)
        {
            var returnme = new Vector3(lhs);
            returnme.x *= rhs;
            returnme.y *= rhs;
            returnme.z *= rhs;
            return returnme;
        }
        public static Vector3 operator * (float lhs, Vector3 rhs)
        {
            return rhs * lhs;
        }
        public static Vector3 operator / (Vector3 lhs, float rhs)
        {
            var returnme = new Vector3(lhs);
            returnme.x /= rhs;
            returnme.y /= rhs;
            returnme.z /= rhs;
            return returnme;
        }

        // easy statics
        public static Vector3 up
        { get { return new Vector3(0, +1, 0); } }
        public static Vector3 down
        { get { return new Vector3(0, -1, 0); } }
        public static Vector3 left
        { get { return new Vector3(-1, 0, 0); } }
        public static Vector3 right
        { get { return new Vector3(+1, 0, 0); } }
        public static Vector3 forward
        { get { return new Vector3(0, 0, -1); } }
        public static Vector3 back
        { get { return new Vector3(0, 0, +1); } }
        public static Vector3 one
        { get { return new Vector3(1, 1, 1); } }
        public static Vector3 zero
        { get { return new Vector3(0, 0, 0); } }
    }
}
