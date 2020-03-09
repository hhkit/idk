using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Rect
    {
        public Vector2 position;
        public Vector2 size;

        public Rect(Vector2 position, Vector2 size)
        {
            this.position = position;
            this.size = size;
        }

        public Rect(float x, float y, float w, float h)
        {
            position.x = x;
            position.y = y;
            size.x = w;
            size.y = h;
        }

        public Vector2 center {
            get => position + size * 0.5f;
            set => position = value - size * 0.5f;
        }
        public Vector2 min {
            get => new Vector2(Mathf.Min(position.x, position.x + size.x), Mathf.Min(position.y, position.y + size.y)); 
            set { size = max - value; position = value; }
        }
        public Vector2 max
        {
            get => new Vector2(Mathf.Max(position.x, position.x + size.x), Mathf.Max(position.y, position.y + size.y));
            set { Vector2 _min = min; size = value - _min; position = _min; }
        }
        public float x { get => position.x; set => position.x = value; }
        public float y { get => position.y; set => position.y = value; }
        public float width { get => size.x; set => size.x = value; }
        public float height { get => size.y; set => size.y = value; }

        public static Rect Lerp(Rect a, Rect b, float t)
            => new Rect(Vector2.Lerp(a.position, b.position, t), Vector2.Lerp(a.size, b.size, t));
        public static Rect LerpUnclamped(Rect a, Rect b, float t)
            => new Rect(Vector2.LerpUnclamped(a.position, b.position, t), Vector2.LerpUnclamped(a.size, b.size, t));

        public bool Contains(Vector2 point)
        {
            Vector2 lmin = min, lmax = max;
            return point.x >= lmin.x && point.x < lmax.x
                && point.y >= lmin.y && point.y < lmax.y;
        }

        public bool Overlaps(Rect other)
        {
            Vector2 lmin = min, rmin = other.min, lmax = max, rmax = other.max;
            return rmax.x > lmin.x &&
                rmin.x < lmax.x &&
                rmax.y > lmin.y &&
                rmin.y < lmax.y;
        }

        public bool Equals(Rect rhs)
        {
            return position == rhs.position && size == rhs.size;
        }
        public static bool operator ==(Rect lhs, Rect rhs)
        {
            return lhs.Equals(rhs);
        }
        public static bool operator !=(Rect lhs, Rect rhs)
        {
            // Returns true in the presence of NaN values.
            return !(lhs == rhs);
        }
        public override bool Equals(object obj)
        {
            return this == (Rect)obj;
        }
        public override int GetHashCode()
        {
            return position.GetHashCode() ^ size.GetHashCode() << 2;
        }
        public override string ToString()
        {
            return string.Format("(X:{0}, Y:{1}, W:{2}, H:{3})", x, y, width, height);
        }

        public static Rect zero { get => new Rect(0, 0, 0, 0); }
    }
}
