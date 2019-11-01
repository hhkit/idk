namespace idk
{
    public class Input
    {
        public static bool GetKeyDown(KeyCode key) => Bindings.InputGetKeyDown((int) key);
        public static bool GetKey(KeyCode key)     => Bindings.InputGetKey((int)key);
        public static bool GetKeyUp(KeyCode key)   => Bindings.InputGetKeyUp((int)key);
        public static float GetAxis(Axis a) => Bindings.InputGetAxis('\0', (int) a);

        /// <summary>
        /// Check if a key is just triggered.
        /// </summary>
        /// <param name="player">The player index to check.</param>
        /// <param name="key">The key for the player to check.</param>
        /// <returns>True if the key was just triggered, false otherwise</returns>
        /// <example>
        /// <code>
        /// if (Input.GetKeyDown(0, KeyCode.JoystickButtonA))
        ///     transform.position += Vector3.up * 50f;
        /// </code></example>
        public static bool GetKeyDown(int player, KeyCode key) 
            => Bindings.InputGetKeyDown((int)key | (player << 8));

        /// <summary>
        /// Check if a key is currently held down.
        /// </summary>
        /// <param name="player">The player index to check.</param>
        /// <param name="key">The key for the player to check.</param>
        /// <returns>True if the key was just triggered, false otherwise</returns>
        /// <example>
        /// <code>
        /// if (Input.GetKey(0, KeyCode.JoystickButtonA))
        ///     transform.position += Vector3.up * 50f;
        /// </code></example>
        public static bool GetKey(int player, KeyCode key)
            => Bindings.InputGetKey((int)key | (player << 8));

        /// <summary>
        /// Check if a key was released.
        /// </summary>
        /// <param name="player">The player index to check.</param>
        /// <param name="key">The key for the player to check.</param>
        /// <returns>True if the key was just triggered, false otherwise</returns>
        /// <example>
        /// <code>
        /// if (Input.GetKeyUp(0, KeyCode.JoystickButtonA))
        ///     transform.position += Vector3.up * 50f;
        /// </code></example>
        public static bool GetKeyUp(int player, KeyCode key)
            => Bindings.InputGetKeyUp((int)key | (player << 8));

        public static float GetAxis(int player, Axis a) => Bindings.InputGetAxis((char) player, (int)a);
    }
}
