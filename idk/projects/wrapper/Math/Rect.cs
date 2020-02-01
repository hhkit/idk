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

        public static Rect zero { get => new Rect(0, 0, 0, 0); }
    }
}
