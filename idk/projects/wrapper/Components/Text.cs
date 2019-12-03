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
        public MaterialInstance materialInstance
        {
            get
            {
                var guid = Bindings.TextGetMaterialInstance(handle);
                MaterialInstance inst = new MaterialInstance();
                inst.guid = guid;
                return inst;
            }
            set => Bindings.TextSetMaterialInstance(handle, (object)value != null ? value.guid : new System.Guid());
        }
        public Color color
        {
            get => Bindings.TextGetColor(handle);
            set => Bindings.TextSetColor(handle, value);
        }
        public uint font_size
        {
            get => Bindings.TextGetFontSize(handle);
            set => Bindings.TextSetFontSize(handle, value);
        }
    }
}