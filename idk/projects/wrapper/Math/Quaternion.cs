﻿using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Quaternion
    {
        public float x;
        public float y;
        public float z;
        public float w;

        // constructors
        public Quaternion(Quaternion q)
        {
            x = q.x;
            y = q.y;
            z = q.z;
            w = q.w;
        }
        public Quaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        // properties
        public Vector3 eulerAngles
        {
            get
            {
                Vector3 v;
                v.x = Mathf.Atan2(w * x + y * z, 0.5f - (x * x + y * y));

                float t2 = 2.0f * (w * y - z * x);
                if (Mathf.Abs(t2) >= 1.0f) // gimbal
                    v.y = (t2 < 0 ? -Mathf.PI : Mathf.PI) * 0.5f * t2;
                else
                    v.y = Mathf.Asin(t2);
                v.z = Mathf.Atan2(w * z + x * y, 0.5f - (y * y + z * z));

                return v;
            }
            set
            {
                float cx = Mathf.Cos(value.x * 0.5f);
		        float sx = Mathf.Sin(value.x * 0.5f);
		        float cy = Mathf.Cos(value.y * 0.5f);
		        float sy = Mathf.Sin(value.y * 0.5f);
		        float cz = Mathf.Cos(value.z * 0.5f);
		        float sz = Mathf.Sin(value.z * 0.5f);

			    x = cz * cy * sx - sz * sy * cx;
			    y = sz * cy * sx + cz * sy * cx;
			    z = sz * cy * cx - cz * sy * sx;
                w = cz * cy * cx + sz * sy * sx;
            }
        }

        public Quaternion normalized
        {
            get
            {
                float s = 1 / Mathf.Sqrt(x * x + y * y + z * z + w * w);
                return new Quaternion(x * s, y * s, z * s, w * s);
            }
        }

        // static methods
        public static float Angle(Quaternion a, Quaternion b)
        {
            //Quaternion q = b * Inverse(a);
            return 0;
        }

        public static Quaternion AngleAxis(float angle, Vector3 axis)
        {
            Quaternion q;
            axis.Normalize();
            float sin = Mathf.Sin(angle * 0.5f);
            q.w = Mathf.Cos(angle * 0.5f);
            q.x = sin * axis.x;
            q.y = sin * axis.y;
            q.z = sin * axis.z;
            return q;
        }

        public static float Dot(Quaternion lhs, Quaternion rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        public static Quaternion Euler(float x, float y, float z)
        {
            float cx = Mathf.Cos(x * 0.5f);
            float sx = Mathf.Sin(x * 0.5f);
            float cy = Mathf.Cos(y * 0.5f);
            float sy = Mathf.Sin(y * 0.5f);
            float cz = Mathf.Cos(z * 0.5f);
            float sz = Mathf.Sin(z * 0.5f);

            return new Quaternion(cz * cy * sx - sz * sy * cx,
                                  sz * cy * sx + cz * sy * cx,
                                  sz * cy * cx - cz * sy * sx,
                                  cz * cy * cx + sz * sy * sx);
        }

        public static Quaternion FromToRotation(Vector3 from, Vector3 to)
        {
            from.Normalize();
            to.Normalize();

            float d = Vector3.Dot(from, to);
            if (d >= 0)
                return identity;
            if (d < -1.0f + 1e-6f)
            {
                // generate arbitrary axis
                Vector3 axis = Vector3.Cross(Vector3.left, from);
                if (axis.sqrMagnitude < 1e-6f) // pick another if colinear
                    axis = Vector3.Cross(Vector3.forward, from);
                return AngleAxis(Mathf.PI, axis);
            }
            { // game programming gems 1, 2.10, Shortest Arc Quaternion, Stan Melax
                Quaternion q;
                float s = Mathf.Sqrt((1 + d) * 2.0f);
                float inv_s = 1.0f / s;
                Vector3 c = Vector3.Cross(from, to);

                q.x = c.x * inv_s;
                q.y = c.y * inv_s;
                q.z = c.z * inv_s;
                q.w = s * 0.5f;
                return Normalize(q);
            }
        }

        public static Quaternion Inverse(Quaternion q)
        {
            q.x = -q.x;
            q.y = -q.y;
            q.z = -q.z;
            return q;
        }
        
        public static Quaternion Lerp(Quaternion a, Quaternion b, float t)
        {
            return LerpUnclamped(a, b, Mathf.Clamp01(t));
        }

        public static Quaternion LerpUnclamped(Quaternion a, Quaternion b, float t)
        {
            a.x = (1 - t) * a.x + t * b.x;
            a.y = (1 - t) * a.y + t * b.y;
            a.z = (1 - t) * a.z + t * b.z;
            a.w = (1 - t) * a.w + t * b.w;
            return a;
        }

        public static Quaternion LookRotation(Vector3 forward)
        {
            return LookRotation(forward, Vector3.up);
        }
        public static Quaternion LookRotation(Vector3 forward, Vector3 up)
        {
            // note: may be wrong

            Quaternion q;
            Vector3 right = Vector3.Cross(up, forward);

            float r00 = right.x;
            float r01 = right.y;
            float r02 = right.z;
            float r10 = up.x;
            float r11 = up.y;
            float r12 = up.z;
            float r20 = forward.x;
            float r21 = forward.y;
            float r22 = forward.z;

            // from geometrictools
            if (r22 <= 0)  // x^2 + y^2 >= z^2 + w^2
            {
                float dif10 = r11 - r00;
                float omr22 = 1.0f - r22;
                if (dif10 <= 0)  // x^2 >= y^2
                {
                    float fourxsqr = omr22 - dif10;
                    float inv4x = 0.5f / Mathf.Sqrt(fourxsqr);
                    q.x = fourxsqr * inv4x;
                    q.y = (r10 + r01) * inv4x;
                    q.z = (r20 + r02) * inv4x;
                    q.w = (r21 - r12) * inv4x;
                }
                else  // y^2 >= x^2
                {
                    float fourysqr = omr22 + dif10;
                    float inv4y = 0.5f / Mathf.Sqrt(fourysqr);
                    q.x = (r10 + r01) * inv4y;
                    q.y = fourysqr * inv4y;
                    q.z = (r20 + r02) * inv4y;
                    q.w = (r21 - r12) * inv4y;
                }
            }
            else  // z^2 + w^2 >= x^2 + y^2
            {
                float sum10 = r11 + r00;
                float opr22 = 1.0f + r22;
                if (sum10 <= 0)  // z^2 >= w^2
                {
                    float fourzsqr = opr22 - sum10;
                    float inv4z = 0.5f / Mathf.Sqrt(fourzsqr);
                    q.x = (r20 + r02) * inv4z;
                    q.y = (r21 + r12) * inv4z;
                    q.z = fourzsqr * inv4z;
                    q.w = (r10 - r01) * inv4z;
                }
                else  // w^2 >= z^2
                {
                    float fourwsqr = opr22 + sum10;
                    float inv4w = 0.5f / Mathf.Sqrt(fourwsqr);
                    q.x = (r21 - r12) * inv4w;
                    q.y = (r02 - r20) * inv4w;
                    q.z = (r10 - r01) * inv4w;
                    q.w = fourwsqr * inv4w;
                }
            }

            return q;
        }

        public static Quaternion Normalize(Quaternion q)
        {
            float s = 1 / Mathf.Sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            q.x *= s;
            q.y *= s;
            q.z *= s;
            q.w *= s;
            return q;
        }

        public static Quaternion RotateTowards(Quaternion from, Quaternion to, Quaternion maxDegreesDelta)
        {
            return identity;
        }

        public static Quaternion Slerp(Quaternion a, Quaternion b, float t)
        {
            return identity;
        }

        public static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t)
        {
            return identity;
        }

        public override bool Equals(object obj)
        {
            return this == (Quaternion)obj;
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
                switch (key)
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
        public static bool operator ==(Quaternion lhs, Quaternion rhs)
        {
            float dot = Dot(lhs, rhs);
            return dot < Mathf.Epsilon && dot > -Mathf.Epsilon;
        }
        public static bool operator !=(Quaternion lhs, Quaternion rhs)
        {
            return !(lhs == rhs);
        }
        public static Quaternion operator *(Quaternion lhs, Quaternion rhs)
        {
            return new Quaternion(
                lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
                lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
                lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z);
        }
        public static Vector3 operator *(Quaternion lhs, Vector3 rhs)
        {
            return new Vector3();
        }

        // easy statics
        public static Quaternion identity
        { get { return new Quaternion(0, 0, 0, 1); } }
    }
}