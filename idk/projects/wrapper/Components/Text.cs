namespace idk
{
    public class Text
        : Component
    {
        public string text
        {
            get => Bindings.TextGetText(handle);
            set => Bindings.TextSetText(handle, value);
        }
        public Color color
        {
            get => Bindings.TextGetColor(handle);
            set => Bindings.TextSetColor(handle, value);
        }
        public uint fontSize
        {
            get => Bindings.TextGetFontSize(handle);
            set => Bindings.TextSetFontSize(handle, value);
        }
    }
}