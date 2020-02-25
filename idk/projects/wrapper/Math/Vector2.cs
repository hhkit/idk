using System;
using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential), Serializable]
    public struct Vector2
    {
        public float x;
        public float y;

        // constructors
        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
        public static implicit operator Vector2(Vector3 v) { return new Vector2(v.x, v.y); }
        public static implicit operator Vector2(Vector4 v) { return new Vector2(v.x, v.y); }

        // properties
        public float sqrMagnitude
        { get { return x * x + y * y; } }
        public float magnitude
        { get { return Mathf.Sqrt(sqrMagnitude);  } }
        public Vector2 normalized
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
                
                return retval;
            }
        }

        // member functions
        public bool Equals(Vector2 rhs)
        {
            return x == rhs.x && y == rhs.y;
        }
        public static Vector3 Normalize(Vector2 v)
        {
            v = v.normalized;
            return v;
        }
        public void Set(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        // static methods
        public static Vector2 ClampMagnitude(Vector2 v, float maxLength)
        {
            if (v.magnitude > maxLength)
                return v.normalized * maxLength;
            else
                return v;
        }
        public static float Distance(Vector2 lhs, Vector2 rhs)
        {
            return (lhs-rhs).magnitude;
        }
        public static float Dot(Vector2 lhs, Vector2 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }
        public static Vector2 Lerp(Vector2 lhs, Vector2 rhs, float t)
        {
            return LerpUnclamped(lhs, rhs, t < 0 ? 0 : t > 1 ? 1 : t);
        }
        public static Vector2 LerpUnclamped(Vector2 lhs, Vector2 rhs, float t)
        {
            return (1-t) * lhs + (t) * rhs;
        }
        public static Vector2 Scale(Vector2 lhs, Vector2 rhs)
        {
            return new Vector2(lhs.x * rhs.x, rhs.y * rhs.y);
        }

        public override bool Equals(object obj)
        {
            return this == (Vector2)obj;
        }
        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2;
        }

        public override string ToString()
        {
            return "(" + x.ToString() + ", " + y.ToString() + ")";
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
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
        }
        public static bool operator ==(Vector2 lhs, Vector2 rhs)
        {
            return Mathf.Abs(lhs.x - rhs.x) < Mathf.Epsilon
                && Mathf.Abs(lhs.y - rhs.y) < Mathf.Epsilon;
        }
        public static bool operator !=(Vector2 lhs, Vector2 rhs)
        {
            return !(lhs == rhs);
        }
        public static Vector2 operator + (Vector2 lhs, Vector2 rhs)
        {
            var returnme = lhs;
            returnme.x += rhs.x;
            returnme.y += rhs.y;
            return returnme;
        }
        public static Vector2 operator - (Vector2 lhs, Vector2 rhs)
        {
            var returnme = lhs;
            returnme.x -= rhs.x;
            returnme.y -= rhs.y;
            return returnme;
        }
        public static Vector2 operator - (Vector2 lhs)
        {
            var returnme = lhs;
            returnme.x = -returnme.x;
            returnme.y = -returnme.y;
            return returnme;
        }
        public static Vector2 operator * (Vector2 lhs, float rhs)
        {
            var returnme = lhs;
            returnme.x *= rhs;
            returnme.y *= rhs;
            return returnme;
        }
        public static Vector2 operator * (float lhs, Vector2 rhs)
        {
            return rhs * lhs;
        }
        public static Vector2 operator / (Vector2 lhs, float rhs)
        {
            var returnme = lhs;
            returnme.x /= rhs;
            returnme.y /= rhs;
            return returnme;
        }

        // easy statics
        public static Vector2 one
        { get { return new Vector2(1, 1); } }
        public static Vector2 zero
        { get { return new Vector2(0, 0); } }
    }
}
