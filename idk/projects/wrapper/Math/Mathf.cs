namespace idk
{
    class Mathf
    {
        public static float PI { get { return 3.14159265358979f; } }
        public static float Epsilon { get { return 1e-6f; } }
        public static float DegToRad { get { return PI / 180; } }
        public static float RadToDeg { get { return 180 / PI; } }

        public static float Abs(float val)
        {
            return val > 0 ? val : -val;
        }
        public static float Floor(float val)
        {
            return (float)System.Math.Floor(val);
        }
        public static float Ceil(float val)
        {
            return (float)System.Math.Ceiling(val);
        }
        public static float Round(float val)
        {
            return (float)System.Math.Round(val);
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
        public static float Acos(float val)
        {
            return (float)System.Math.Acos(val);
        }
        public static float Max(float a, float b)
        {
            return a > b ? a : b;
        }
        public static float Max(params float[] values)
        {
            for (int i = 1; i < values.Length; i++)
            {
                if (values[i] > values[0])
                    values[0] = values[i];
            }
            return values[0];
        }
        public static float Min(float a, float b)
        {
            return a < b ? a : b;
        }
        public static float Min(params float[] values)
        {
            for (int i = 1; i < values.Length; i++)
            {
                if (values[i] < values[0])
                    values[0] = values[i];
            }
            return values[0];
        }
        public static int Max(int a, int b)
        {
            return a > b ? a : b;
        }
        public static int Max(params int[] values)
        {
            for (int i = 1; i < values.Length; i++)
            {
                if (values[i] > values[0])
                    values[0] = values[i];
            }
            return values[0];
        }
        public static int Min(int a, int b)
        {
            return a < b ? a : b;
        }
        public static int Min(params int[] values)
        {
            for (int i = 1; i < values.Length; i++)
            {
                if (values[i] < values[0])
                    values[0] = values[i];
            }
            return values[0];
        }
    }
}
