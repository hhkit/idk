using System;
using System.Collections.Generic;

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
        /// Random float in range [min, max]. (Including min and max)
        /// </summary>
        public static float Range(float min, float max) { return Mathf.LerpUnclamped(min, max, value); }

        /// <summary>
        /// Random integer in range [min, max). (Including min, excluding max)
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

        public static void Shuffle<T>(T[] array)
        {
            for (int i = 0; i < array.Length - 1; i++)
            {
                int j = Range(i, array.Length);
                T tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }

        public static void Shuffle<T>(IList<T> list)
        {
            for (int i = 0; i < list.Count - 1; i++)
            {
                int j = Range(i, list.Count);
                T tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }

    public static class RandomExtensions
    {
        public static T[] Shuffle<T>(this T[] array) { Random.Shuffle(array); return array; }
        public static IList<T> Shuffle<T>(this IList<T> list) { Random.Shuffle(list); return list; }
    }
}
