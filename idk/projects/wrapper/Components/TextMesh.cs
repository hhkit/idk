namespace idk
{
    public class TextMesh
        : Component
    {
        public string text
        {
            get => Bindings.TextMeshGetText(handle);
            set => Bindings.TextMeshSetText(handle, value);
        }

        public Color color
        {
            get => Bindings.TextMeshGetColor(handle);
            set => Bindings.TextMeshSetColor(handle, value);
        }
    }
}