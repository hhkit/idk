using System;

namespace idk
{
    public static class Random
    {
        static System.Random rand = new System.Random();

        public static void InitState(int seed) { rand = new System.Random(seed); }

        /// <summary>
        /// Random integer in range [0.0f, 1.0f].
        /// </summary>
        public static float value { get => (float)((double)rand.Next() / int.MaxValue); }

        /// <summary>
        /// Random integer in range [min, max].
        /// </summary>
        public static float Range(float min, float max) { return Mathf.LerpUnclamped(min, max, value); }

        /// <summary>
        /// Random integer in range [min, max).
        /// </summary>
        public static int Range(int min, int max) { return rand.Next(min, max); }

        /// <summary>
        /// Approximated gaussian distribution in the range ~[mean - x, mean + x].
        /// By default, ~[-3, 3].
        /// </summary>
        public static float Gaussian(float mean = 0, float x = 3.0f)
        {
            float gauss =
                value + value + value + value + value + value +
                value + value + value + value + value + value - 6;
            return mean + gauss / 3.0f * x;
        }
    }
}
