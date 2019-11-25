namespace idk
{
    public class Image
        : Component
    {
        public Texture texture
        {
            get
            {
                var guid = Bindings.ImageGetTexture(handle);
                Texture inst = new Texture();
                inst.guid = guid;
                return inst;
            }
            set => Bindings.ImageSetTexture(handle, (object)value != null ? value.guid : new System.Guid());
        }
        public MaterialInstance materialInstance
        {
            get
            {
                var guid = Bindings.ImageGetMaterialInstance(handle);
                MaterialInstance inst = new MaterialInstance();
                inst.guid = guid;
                return inst;
            }
            set => Bindings.ImageSetMaterialInstance(handle, (object)value != null ? value.guid : new System.Guid());
        }
        public Color color
        {
            get => Bindings.ImageGetColor(handle);
            set => Bindings.ImageSetColor(handle, value);
        }
    }
}