namespace idk
{
    public class Input
    {
        /// <summary>
        /// Returns true during the frame the user starts pressing down the key.
        /// </summary>
        public static bool GetKeyDown(KeyCode key) => Bindings.InputGetKeyDown((int) key);

        /// <summary>
        /// Returns true while the user holds down the key.
        /// </summary>
        public static bool GetKey(KeyCode key)     => Bindings.InputGetKey((int)key);

        /// <summary>
        /// Returns true during the frame the user releases the key.
        /// </summary>
        public static bool GetKeyUp(KeyCode key)   => Bindings.InputGetKeyUp((int)key);

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

        /// <summary>
        /// Returns the value of the gamepad axis.
        /// </summary>
        public static float GetAxis(int player, Axis a) => Bindings.InputGetAxis((sbyte)player, (int)a);

        /// <summary>
        /// Sets the gamepad's rumble.
        /// </summary>
        /// <param name="player">The player index</param>
        /// <param name="low_freq">The value of the low-frequency motor, from 0 to 1.</param>
        /// <param name="high_freq">The value of the high-frequency motor, from 0 to 1.</param>
        /// <returns></returns>
        public static float SetRumble(int player, float low_freq, float high_freq) => Bindings.InputSetRumble((sbyte)player, low_freq, high_freq);

        public static int[] connectedPlayers
        {
            get
            {
                byte mask = Bindings.InputGetConnectedPlayers();
                switch(mask)
                {
                    case 0b0001: return new int[] { 0 };
                    case 0b0010: return new int[] { 1 };
                    case 0b0100: return new int[] { 2 };
                    case 0b1000: return new int[] { 3 };
                    case 0b0011: return new int[] { 0, 1 };
                    case 0b0101: return new int[] { 0, 2 };
                    case 0b1001: return new int[] { 0, 3 };
                    case 0b0110: return new int[] { 1, 2 };
                    case 0b1010: return new int[] { 1, 3 };
                    case 0b1100: return new int[] { 2, 3 };
                    case 0b0111: return new int[] { 0, 1, 2 };
                    case 0b1011: return new int[] { 0, 1, 3 };
                    case 0b1101: return new int[] { 0, 2, 3 };
                    case 0b1110: return new int[] { 1, 2, 3 };
                    case 0b1111: return new int[] { 0, 1, 2, 3 };
                    default: return new int[] { };
                }
            }
        }
    }
}
