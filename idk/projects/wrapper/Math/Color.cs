using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Color
    {
        public float r;
        public float g;
        public float b;
        public float a;

        // constructors
        public Color(Vector4 v) : this(v.x, v.y, v.z, v.w) { }
        public Color(float r, float g, float b, float a = 1)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }
        public static implicit operator Color(Vector4 v) { return new Color(v); }
        public static implicit operator Vector4(Color c) { return new Vector4(c.r, c.g, c.b, c.a); }

        // properties
        public Color gamma
        { get { return this; } }
        public Color linear
        { get { return this; } }
        public Color grayscale
        { get { return this; } }

        // member functions
        public bool Equals(Color rhs)
        {
            return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
        }

        // static methods
        public static Color Lerp(Color lhs, Color rhs, float t)
        {
            return LerpUnclamped(lhs, rhs, t < 0 ? 0 : t > 1 ? 1 : t);
        }
        public static Color LerpUnclamped(Color lhs, Color rhs, float t)
        {
            return (1 - t) * lhs + (t) * rhs;
        }
        public static void RGBToHSV(Color rgb, out float h, out float s, out float v)
        {
            var max = Mathf.Max(rgb.r, rgb.g, rgb.b);
            var min = Mathf.Min(rgb.r, rgb.g, rgb.b);
            v = max;

            var d = max - min;
            s = max == 0 ? 0 : d / max;

            if (max == min)
                h = 0; // achromatic
            else if (max == rgb.r)
                h = ((rgb.g - rgb.b) / d + (rgb.g < rgb.b ? 6 : 0)) / 6;
            else if (max == rgb.g)
                h = ((rgb.b - rgb.r) / d + 2) / 6;
            else
                h = ((rgb.r - rgb.g) / d + 4) / 6;
        }
        public static Color HSVToRGB(float h, float s, float v)
        {
            var i = Mathf.Floor(h * 6);
            var f = h * 6 - i;
            var p = v * (1 - s);
            var q = v * (1 - f * s);
            var t = v * (1 - (1 - f) * s);

            switch (i % 6)
            {
                case 0: return new Color(v, t, p);
                case 1: return new Color(q, v, p);
                case 2: return new Color(p, v, t);
                case 3: return new Color(p, q, v);
                case 4: return new Color(t, p, v);
                case 5: return new Color(v, p, q);
                default: return new Color();
            }
        }

        public override bool Equals(object obj)
        {
            return this == (Color)obj;
        }
        public override int GetHashCode()
        {
            return (r.GetHashCode() ^ g.GetHashCode() << 2) ^ (b.GetHashCode() ^ a.GetHashCode() << 2) << 2;
        }

        // operator overloads
        public float this[int key]
        {
            get
            {
                switch (key)
                {
                    case 0: return r;
                    case 1: return g;
                    case 2: return b;
                    case 3: return a;
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
            set
            {
                switch (key)
                {
                    case 0: r = value; break;
                    case 1: g = value; break;
                    case 2: b = value; break;
                    case 3: a = value; break;
                    default:
                        throw new System.IndexOutOfRangeException { };
                }
            }
        }
        public static bool operator ==(Color lhs, Color rhs)
        {
            return lhs.Equals(rhs);
        }
        public static bool operator !=(Color lhs, Color rhs)
        {
            return !(lhs == rhs);
        }
        public static Color operator +(Color lhs, Color rhs)
        {
            var col = lhs;
            col.r += rhs.r;
            col.g += rhs.g;
            col.b += rhs.b;
            return col;
        }
        public static Color operator -(Color lhs, Color rhs)
        {
            var col = lhs;
            col.r -= rhs.r;
            col.g -= rhs.g;
            col.b -= rhs.b;
            return col;
        }
        public static Color operator *(Color lhs, Color rhs)
        {
            var col = lhs;
            col.r *= rhs.r;
            col.g *= rhs.g;
            col.b *= rhs.b;
            return col;
        }
        public static Color operator *(Color lhs, float rhs)
        {
            var col = lhs;
            col.r *= rhs;
            col.g *= rhs;
            col.b *= rhs;
            return col;
        }
        public static Color operator *(float lhs, Color rhs)
        {
            return rhs * lhs;
        }
        public static Color operator /(Color lhs, float rhs)
        {
            var col = lhs;
            col.r /= rhs;
            col.g /= rhs;
            col.b /= rhs;
            return col;
        }

        public static Color white
        { get { return new Color(1, 1, 1); } }
        public static Color black
        { get { return new Color(0, 0, 0); } }
        public static Color red
        { get { return new Color(1, 0, 0); } }
        public static Color green
        { get { return new Color(0, 1, 0); } }
        public static Color blue
        { get { return new Color(0, 0, 1); } }
        public static Color clear
        { get { return new Color(0, 0, 0, 0); } }
    }
}
