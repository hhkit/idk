namespace idk
{
    public class Mathf
    {
        public static float PI { get { return 3.14159265358979f; } }
        public static float Tau { get { return 6.28318530717958f; } }
        public static float Epsilon { get { return 1e-6f; } }
        public static float DegToRad { get { return PI / 180; } }
        public static float RadToDeg { get { return 180 / PI; } }

        public const float Infinity = float.PositiveInfinity;
        public const float NegativeInfinity = float.NegativeInfinity;

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
        public static int Clamp(int val, int min, int max)
        {
            return val < min ? min :
                   val > max ? max : val;
        }
        public static float Clamp01(float val)
        {
            return val < 0 ? 0 :
                   val > 1 ? 1 : val;
        }
        public static float Lerp(float a, float b, float t)
        {
            return LerpUnclamped(a, b, t < 0 ? 0 : t > 1 ? 1 : t);
        }
        public static float LerpUnclamped(float a, float b, float t)
        {
            return (1 - t) * a + (t) * b;
        }
        public static float Pow(float f, float p)
        {
            return (float)System.Math.Pow(f, p);
        }
        public static float Sin(float val)
        {
            return (float)System.Math.Sin(val);
        }
        public static float Cos(float val)
        {
            return (float)System.Math.Cos(val);
        }
        public static float Tan(float val)
        {
            return (float)System.Math.Tan(val);
        }
        public static float Atan(float val)
        {
            return (float)System.Math.Atan(val);
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

        /// <summary>
        /// Samples Perlin Noise at (x,y), returns [0, 1] (0.5 at integer points).
        /// </summary>
        public static float PerlinNoise(float x, float y)
        {
            return Perlin.Noise(x, y);
        }

        /// <summary>
        /// Samples Perlin Noise at (x,y) using multiple octaves, returns [0, 1].
        /// Note that the more octaves used, the more "compressed" the values will be.
        /// </summary>
        /// <param name="octaves">Number of octaves.</param>
        /// <param name="persistence">The amplitude multiplier for each successive octave.</param>
        /// <param name="lacunarity">The frequency multiplier for each successive octave.</param>
        public static float PerlinNoise(float x, float y, int octaves, float persistence, float lacunarity)
        {
            return Perlin.Noise(x, y);
        }
    }

    internal static class Perlin
    {
        private static int[] perm = new int[]{ 151,160,137,91,90,15,
            131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
            190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
            88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
            77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
            102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
            135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
            5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
            223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
            129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
            251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
            49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
            151,160,137,91,90,15,
            131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
            190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
            88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
            77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
            102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
            135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
            5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
            223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
            129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
            251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
            49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

        private static Vector2[] grads = new Vector2[] {
            new Vector2(-1, -1), new Vector2(1, -1), new Vector2(-1, 1), new Vector2(1, 1) };

        private static float Fade(float t)
        {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        /// <summary>
        /// Samples Perlin Noise at (x,y), returns [0, 1] (0.5 at integer points).
        /// </summary>
        public static float Noise(float x, float y)
        {
            // Find unit grid cell containing point
            int X = (int)Mathf.Floor(x), Y = (int)Mathf.Floor(y);
            // Get relative xy coordinates of point within that cell
            x -= X; y -= Y;
            // Wrap the integer cells at 255 (smaller integer period can be introduced here)
            X &= 255; Y &= 255;

            // Calculate noise contributions from each of the four corners
            var n00 = Vector2.Dot(grads[X + perm[Y]], new Vector2(x, y));
            var n01 = Vector2.Dot(grads[X + perm[Y + 1]], new Vector2(x, y - 1));
            var n10 = Vector2.Dot(grads[X + 1 + perm[Y]], new Vector2(x - 1, y));
            var n11 = Vector2.Dot(grads[X + 1 + perm[Y + 1]], new Vector2(x - 1, y - 1));

            // Compute the fade curve value for x
            var u = Fade(x);

            // Interpolate the four results
            return Mathf.LerpUnclamped(
                Mathf.LerpUnclamped(n00, n10, u),
                Mathf.LerpUnclamped(n01, n11, u),
                Fade(y)) + 0.5f;
        }

        /// <summary>
        /// Samples Perlin Noise at (x,y) using multiple octaves, returns [0, 1].
        /// Note that the more octaves used, the more "compressed" the values will be.
        /// </summary>
        /// <param name="octaves">Number of octaves.</param>
        /// <param name="persistence">The amplitude multiplier for each successive octave.</param>
        /// <param name="lacunarity">The frequency multiplier for each successive octave.</param>
        public static float Noise(float x, float y, int octaves, float persistence, float lacunarity)
        {
            float total = 0;
            float maxValue = 0;
            float amplitude = 1.0f;
            float frequency = 1.0f;

            for (int i = 0; i < octaves; i++)
            {
                total += Noise(x * frequency, y * frequency) * amplitude;
                maxValue += amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }

            return total / maxValue;
        }
    }
}
