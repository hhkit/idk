namespace idk
{
    public class Time
    {
        /// <summary>
        ///     The completion time in seconds since the last frame (Read Only).
        /// </summary>
        public static float deltaTime
        { get => Bindings.TimeGetDelta(); }

        /// <summary>
        ///    The interval in seconds at which physics and other fixed frame rate updates (like MonoBehaviour's FixedUpdate) are performed.
        /// </summary>
        public static float fixedDeltaTime
        { get => Bindings.TimeGetFixedDelta(); }


        /// <summary>
        ///     The completion time in seconds since the last frame (Read Only).
        /// </summary>
        public static float unscaledDeltaTime
        { get => Bindings.TimeGetUnscaledDelta(); }

        /// <summary>
        ///    The interval in seconds at which physics and other fixed frame rate updates (like MonoBehaviour's FixedUpdate) are performed.
        /// </summary>
        public static float unscaledFixedDeltaTime
        { get => Bindings.TimeGetUnscaledFixedDelta(); }

        /// <summary>
        ///     Time elapsed since program startup in seconds, unaffected by timeScale.
        /// </summary>
        public static float realtimeSinceStartup
        { get => Bindings.TimeGetTimeSinceStart(); }

        /// <summary>
        ///     The scale at which the time is passing. This can be used for slow motion effects.
        /// </summary>
        public static float timeScale
        {
            get => Bindings.TimeGetTimeScale();
            set => Bindings.TimeSetTimeScale(value);
        }


        public static float accumulatedTime
        {
            get => Bindings.TimeGetAccumTime();
        }

        //
        //public static float unscaledTime
        //{
        //    get { return 0; }
        //}
    }
}
