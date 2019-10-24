namespace idk
{
    public class Input
    {
        public static bool GetKeyDown(KeyCode key) => Bindings.InputGetKeyDown((int) key);
        public static bool GetKey(KeyCode key)     => Bindings.InputGetKey((int)key);
        public static bool GetKeyUp(KeyCode key)   => Bindings.InputGetKeyUp((int)key);
        public static float GetAxis(Axis a) => Bindings.InputGetAxis('\0', (int) a);
    }
}
