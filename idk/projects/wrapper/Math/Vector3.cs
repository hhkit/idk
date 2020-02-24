using System;
using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential), Serializable]
    public struct Vector3
    {
        public float x;
        public float y;
        public float z;

        // constructors
        public Vector3(float x, float y, float z = 0)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
        public static implicit operator Vector3(Vector2 v) { return new Vector3(v.x, v.y, 0); }
        public static implicit operator Vector3(Vector4 v) { return new Vector3(v.x, v.y, v.z); }

        // properties
        public float sqrMagnitude
        { get { return x * x + y * y + z * z; } }
        public float magnitude
        { get { return Mathf.Sqrt(sqrMagnitude);  } }
        public Vector3 normalized
        {
            get
            {
                Vector3 retval = this;
                var mag = magnitude;
                if (mag < 0.001f)
                {
                    return Vector3.zero;
                }
                retval.x /= mag;
                retval.y /= mag;
                retval.z /= mag;
                return retval;
            }
        }
        
        // member functions
        public bool Equals(Vector3 rhs)
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
        public static Vector3 Normalize(Vector3 v)
        {
            v = v.normalized;
            return v;
        }
        public void Set(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        // static methods
        public static Vector3 ClampMagnitude(Vector3 v, float maxLength)
        {
            if (v.magnitude > maxLength)
                return v.normalized * maxLength;
            else
                return v;
        }
        public static float Distance(Vector3 lhs, Vector3 rhs)
        {
            return (lhs-rhs).magnitude;
        }
        public static float Dot(Vector3 lhs, Vector3 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }
        public static Vector3 Cross(Vector3 lhs, Vector3 rhs)
        {
            return new Vector3(lhs[1] * rhs[2] - rhs[1] * lhs[2],
                               lhs[2] * rhs[0] + rhs[2] * lhs[0],
                               lhs[0] * rhs[1] - rhs[0] * lhs[1]);
        }

        public static Vector3 Reflect(Vector3 inDirection, Vector3 inNormal)
        {
            return inDirection - 2.0f * Dot(inDirection, inNormal) * inNormal;
        }

        public static Vector3 Refract(Vector3 inDirection, Vector3 inNormal, float eta)
        {
            float norm_dot_dir = Dot(inDirection, inNormal);
            float k = 1.0f - eta * eta * (1.0f - norm_dot_dir * norm_dot_dir);
            Vector3 ret_val = new Vector3( 0, 0, 0 );
            if (k >= 0.0f)
               ret_val = eta * inDirection - (eta * norm_dot_dir + Mathf.Sqrt(k)) * inNormal; 
            return ret_val;
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

        public override string ToString()
        {
            return "(" + x.ToString() + ", " + y.ToString() + ", " + z.ToString() + ")";
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
            var returnme = lhs;
            returnme.x += rhs.x;
            returnme.y += rhs.y;
            returnme.z += rhs.z;
            return returnme;
        }
        public static Vector3 operator - (Vector3 lhs, Vector3 rhs)
        {
            var returnme = lhs;
            returnme.x -= rhs.x;
            returnme.y -= rhs.y;
            returnme.z -= rhs.z;
            return returnme;
        }
        public static Vector3 operator - (Vector3 lhs)
        {
            var returnme = lhs;
            returnme.x = -returnme.x;
            returnme.y = -returnme.y;
            returnme.z = -returnme.z;
            return returnme;
        }
        public static Vector3 operator * (Vector3 lhs, float rhs)
        {
            var returnme = lhs;
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
            var returnme = lhs;
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
