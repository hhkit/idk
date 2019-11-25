namespace idk
{
    public class TextMesh
        : Component
    {
        public string Text
        {
            get => Bindings.TextMeshGetText(handle);
            set => Bindings.TextMeshSetText(handle, value);
        }

        public Color Color
        {
            get => Bindings.TextMeshGetColor(handle);
            set => Bindings.TextMeshSetColor(handle, value);
        }
    }
}