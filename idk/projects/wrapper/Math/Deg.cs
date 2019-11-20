using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Deg
    {
        public float value;
        public Deg(Deg val)
        {
            this = val;
        }
        public Deg(float val)
        {
            this.value = val;
        }

        public Deg(Rad val)
        {
            this.value = val / (180f * Mathf.PI);
        }

        public Deg Normalize()
        {
            value = value % 360f;
            if (value > 180f)
                value -= 360f;

            return this;
        }

        public void ConvertTo(Rad val)
        {
            this.value = val / (180f * Mathf.PI);

            //return this;
        }

        public float Data()
        {
            return value;
        }

        public static bool operator ==(Deg lhs, Deg rhs)
        {
            return Mathf.Abs(lhs.value - rhs.value) < Mathf.Epsilon;
        }
        public static bool operator !=(Deg lhs, Deg rhs)
        {
            return !(lhs == rhs);
        }

        public static bool operator <(Deg lhs, Deg rhs)
        {
            return lhs.value < rhs.value;
        }

        public static bool operator >(Deg lhs, Deg rhs)
        {
            return lhs.value > rhs.value;
        }

        public static Deg operator +(Deg lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value + rhs;

            return returnme;
        }

        public static Deg operator -(Deg lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value - rhs;

            return returnme;
        }

        public static Deg operator +(Deg lhs, Deg rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value + rhs.value;

            return returnme;
        }

        public static Deg operator -(Deg lhs, Deg rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value - rhs.value;

            return returnme;
        }

        public static Deg operator -(Deg lhs)
        {
            var returnme = -lhs;

            return returnme;
        }

        public static float operator /(Deg lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs;

            return returnme.value;
        }

        public static Deg operator /(Deg lhs, Deg rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs.value;

            return returnme;
        }

        public static float operator *(Deg lhs, float rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs;

            return returnme.value;
        }

        public static Deg operator *(Deg lhs, Deg rhs)
        {
            var returnme = lhs;
            returnme.value = lhs.value / rhs.value;

            return returnme;
        }

        public static float Cos(Deg lhs)
        {
            Rad val;
            val.value = 0f;
            val.ConvertTo(lhs);
            
            return Rad.Cos(val);
        }

        public static float Sin(Deg lhs)
        {
            Rad val;
            val.value = 0f;
            val.ConvertTo(lhs);

            return Rad.Sin(val);
        }

        public static float Tan(Deg lhs)
        {
            Rad val;
            val.value = 0f;
            val.ConvertTo(lhs);

            return Rad.Tan(val);
        }

    }
}
