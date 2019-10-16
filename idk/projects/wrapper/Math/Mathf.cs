namespace idk
{
    class Mathf
    {
        public static float PI { get { return 3.1415927f; } }

        public static float Epsilon { get { return 1e-6f; } }

        public static float Abs(float val)
        {
            return val > 0 ? val : -val;
        }
        public static float Sqrt(float val)
        {
            return (float)System.Math.Sqrt(val);
        }
        public static float Clamp(float val, float min, float max)
        {
            return val < min ? min :
                   val > max ? max : val;
        }
        public static float Clamp01(float val)
        {
            return val < 0 ? 0 :
                   val > 1 ? 1 : val;
        }
        public static float Sin(float val)
        {
            return (float)System.Math.Sin(val);
        }
        public static float Cos(float val)
        {
            return (float)System.Math.Cos(val);
        }
        public static float Atan2(float y, float x)
        {
            return (float)System.Math.Atan2(y, x);
        }
        public static float Asin(float val)
        {
            return (float)System.Math.Asin(val);
        }
    }
}
