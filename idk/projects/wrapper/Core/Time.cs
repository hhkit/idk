namespace idk
{
    public class Time
    {
        /// <summary>
        ///     Frame time step
        /// </summary>
        public static float deltaTime
        { get => Bindings.TimeGetDelta(); }

        /// <summary>
        ///     Fixed timestep
        /// </summary>
        public static float fixedDeltaTime
        { get => Bindings.TimeGetFixedDelta(); }

        ///// <summary>
        /////     Time elapsed since program start in seconds
        ///// </summary>
        //public static float elapsedTime
        //{
        //    get { return 0; }
        //}
        //
        //public static float timeScale
        //{
        //    get { return 0; }
        //}
        //
        //public static float unscaledTime
        //{
        //    get { return 0; }
        //}
    }
}
