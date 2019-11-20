using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Rad
    {
        public float value;
        public Rad(Rad val)
        {
            this = val;
        }
        public Rad(float val)
        {
            this.value = val;
        }

        public Rad(Deg val)
        {
            this.value = val / Mathf.PI * 180f;
        }

        public Rad Normalize()
        {
            value = value % Mathf.Tau;
            if (value > Mathf.PI)
			    value -= Mathf.Tau;

            return this;
        }

        public void ConvertTo(Deg val)
        {
            this.value = val / Mathf.PI * 180f;

            //return this;
        }

        public float Data()
        {
            return value;
        }

        public static bool operator == (Rad lhs, Rad rhs)
        {
            return Mathf.Abs(lhs.value - rhs.value) < Mathf.Epsilon;
        }
        public static bool operator !=(Rad lhs, Rad rhs)
        {
            return !(lhs == rhs);
        }

        public static bool operator <(Rad lhs, Rad rhs)
        {
            return lhs.value < rhs.value;
        }

        public static bool operator >(Rad lhs, Rad rhs)
        {
            return lhs.value > rhs.value;
        }

        public static Rad operator + (Rad lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value + rhs;

            return returnme;
        }

        public static Rad operator - (Rad lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value - rhs;

            return returnme;
        }

        public static Rad operator +(Rad lhs, Rad rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value + rhs.value;

            return returnme;
        }

        public static Rad operator -(Rad lhs, Rad rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value - rhs.value;

            return returnme;
        }

        public static Rad operator -(Rad lhs)
        {
            var returnme = -lhs;

            return returnme;
        }

        public static float operator / (Rad lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs;
 
            return returnme.value;
        }

        public static Rad operator /(Rad lhs, Rad rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs.value;

            return returnme;
        }

        public static float operator *(Rad lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs;

            return returnme.value;
        }

        public static Rad operator *(Rad lhs, Rad rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs.value;

            return returnme;
        }

        public static float Cos(Rad lhs)
        {
            return Mathf.Cos(lhs.value);
        }

        public static float Sin(Rad lhs)
        {
            return Mathf.Sin(lhs.value);
        }

        public static float Tan(Rad lhs)
        {
            return Mathf.Tan(lhs.value);
        }
        public static Rad Acos(Rad lhs)
        {
            Rad ret;
            ret.value = Mathf.Acos(lhs.value);
            return ret;
        }

        public static Rad Asin(Rad lhs)
        {
            Rad ret;
            ret.value = Mathf.Asin(lhs.value);
            return ret;
        }

        public static Rad Atan(Rad lhs)
        {
            Rad ret;
            ret.value = Mathf.Atan(lhs.value);
            return ret;
        }

        public static Rad Atan2(Rad lhs, Rad rhs)
        {
            Rad ret;
            ret.value = Mathf.Atan2(lhs.value, rhs.value);
            return ret;
        }

    }
}
