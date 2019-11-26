namespace idk
{
    public class Input
    {
        public static bool GetKeyDown(KeyCode key) => Bindings.InputGetKeyDown((int) key);
        public static bool GetKey(KeyCode key)     => Bindings.InputGetKey((int)key);
        public static bool GetKeyUp(KeyCode key)   => Bindings.InputGetKeyUp((int)key);
        public static float GetAxis(Axis a)        => Bindings.InputGetAxis(0, (int) a);

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

        public static float GetAxis(int player, Axis a) => Bindings.InputGetAxis((sbyte)player, (int)a);

        public static int[] ConnectedPlayers
        {
            get
            {
                byte mask = Bindings.InputGetConnectedPlayers();
                switch(mask)
                {
                    case 0b0001: return new int[] { 1 };
                    case 0b0010: return new int[] { 2 };
                    case 0b0100: return new int[] { 3 };
                    case 0b1000: return new int[] { 4 };
                    case 0b0011: return new int[] { 1, 2 };
                    case 0b0101: return new int[] { 1, 3 };
                    case 0b1001: return new int[] { 1, 4 };
                    case 0b0110: return new int[] { 2, 3 };
                    case 0b1010: return new int[] { 2, 4 };
                    case 0b1100: return new int[] { 3, 4 };
                    case 0b0111: return new int[] { 1, 2, 3 };
                    case 0b1011: return new int[] { 1, 2, 4 };
                    case 0b1101: return new int[] { 1, 3, 4 };
                    case 0b1110: return new int[] { 2, 3, 4 };
                    case 0b1111: return new int[] { 1, 2, 3, 4 };
                    default: return new int[] { };
                }
            }
        }
    }
}
