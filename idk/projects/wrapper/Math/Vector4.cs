using System;
using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential), Serializable]
    public struct Vector4
    {
        public float x;
        public float y;
        public float z;
        public float w;

        // constructors
        public Vector4(float x, float y, float z = 0, float w = 0)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
        public static implicit operator Vector4(Vector2 v) { return new Vector4(v.x, v.y, 0, 0); }
        public static implicit operator Vector4(Vector3 v) { return new Vector4(v.x, v.y, v.z, 0); }

        // properties
        public float sqrMagnitude
        { get { return x * x + y * y + z * z + w * w; } }
        public float magnitude
        { get { return Mathf.Sqrt(sqrMagnitude);  } }
        public Vector4 normalized
        {
            get
            {
                Vector4 retval = this;
                var mag = magnitude;
                if (mag < 0.001f)
                {
                    return Vector4.zero;
                }
                retval.x /= mag;
                retval.y /= mag;
                retval.z /= mag;
                retval.w /= mag;

                return retval;
            }
        }

        // member functions
        public bool Equals(Vector4 rhs)
        {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }
        public static Vector4 Normalize(Vector4 v)
        {
            v = v.normalized;
            return v;
        }

        public void Set(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        // static methods
        public static Vector4 ClampMagnitude(Vector4 v, float maxLength)
        {
            if (v.magnitude > maxLength)
                return v.normalized * maxLength;
            else
                return v;
        }
        public static float Distance(Vector4 lhs, Vector4 rhs)
        {
            return (lhs-rhs).magnitude;
        }
        public static float Dot(Vector4 lhs, Vector4 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }
        public static Vector4 Lerp(Vector4 lhs, Vector4 rhs, float t)
        {
            return LerpUnclamped(lhs, rhs, t < 0 ? 0 : t > 1 ? 1 : t);
        }
        public static Vector4 LerpUnclamped(Vector4 lhs, Vector4 rhs, float t)
        {
            return (1-t) * lhs + (t) * rhs;
        }
        public static Vector4 Scale(Vector4 lhs, Vector4 rhs)
        {
            return new Vector4(lhs.x * rhs.x, rhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        }

        public override bool Equals(object obj)
        {
            return this == (Vector4)obj;
        }
        public override int GetHashCode()
        {
            return (x.GetHashCode() ^ y.GetHashCode() << 2) ^ (z.GetHashCode() ^ w.GetHashCode() << 2) << 2;
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
                    case 3: return w;
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
            set
            {
                switch (key)
                {
                    case 0: x = value; break;
                    case 1: y = value; break;
                    case 2: z = value; break;
                    case 3: w = value; break;
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
        }
        public static bool operator ==(Vector4 lhs, Vector4 rhs)
        {
            float dot = Dot(lhs, rhs);
            return dot < Mathf.Epsilon && dot > -Mathf.Epsilon;
        }
        public static bool operator !=(Vector4 lhs, Vector4 rhs)
        {
            return !(lhs == rhs);
        }
        public static Vector4 operator + (Vector4 lhs, Vector4 rhs)
        {
            var returnme = lhs;
            returnme.x += rhs.x;
            returnme.y += rhs.y;
            returnme.z += rhs.z;
            returnme.w += rhs.w;
            return returnme;
        }
        public static Vector4 operator - (Vector4 lhs, Vector4 rhs)
        {
            var returnme = lhs;
            returnme.x -= rhs.x;
            returnme.y -= rhs.y;
            returnme.z -= rhs.z;
            returnme.w -= rhs.w;
            return returnme;
        }
        public static Vector4 operator - (Vector4 lhs)
        {
            var returnme = lhs;
            returnme.x = -returnme.x;
            returnme.y = -returnme.y;
            returnme.z = -returnme.z;
            returnme.w = -returnme.w;
            return returnme;
        }
        public static Vector4 operator * (Vector4 lhs, float rhs)
        {
            var returnme = lhs;
            returnme.x *= rhs;
            returnme.y *= rhs;
            returnme.z *= rhs;
            returnme.w *= rhs;
            return returnme;
        }
        public static Vector4 operator * (float lhs, Vector4 rhs)
        {
            return rhs * lhs;
        }
        public static Vector4 operator / (Vector4 lhs, float rhs)
        {
            var returnme = lhs;
            returnme.x /= rhs;
            returnme.y /= rhs;
            returnme.z /= rhs;
            returnme.w /= rhs;
            return returnme;
        }

        public static Vector4 one
        { get { return new Vector4(1, 1, 1, 1); } }
        public static Vector4 zero
        { get { return new Vector4(0, 0, 0, 0); } }
    }
}
