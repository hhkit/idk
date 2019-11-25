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
        }
        public Color color
        {
            get => Bindings.ImageGetColor(handle);
            set => Bindings.ImageSetColor(handle, value);
        }
    }
}