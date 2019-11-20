namespace idk
{
    public class Font
        : Component
    {
        public string Text
        {
            get => Bindings.FontGetText(handle);
            set => Bindings.FontSetText(handle, value);
        }

        public Color FontColor
        {
            get => Bindings.FontGetColor(handle);
            set => Bindings.FontSetColor(handle, value);
        }
    }
}