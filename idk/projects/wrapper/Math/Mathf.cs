namespace idk
{
    class Mathf
    {
        public static float PI { get { return 3.1415927f; } }

        public static float Epsilon { get { return 0.0001f; } }

        public static float Abs(float val)
        {
            return val > 0 ? val : -val;
        }
        public static float Sqrt(float val)
        {
            return (float)System.Math.Sqrt(val);
        }
    }
}
