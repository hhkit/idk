namespace idk
{
    public class Renderer
        : Component
    {
        public MaterialInstance materialInstance
        {
            get
            {
                var guid = Bindings.RendererGetMaterialInstance(handle);
                MaterialInstance inst = new MaterialInstance();
                inst.guid = guid;
                return inst;
            }
            set
            {
                Bindings.RendererSetMaterialInstance(handle, (object)value != null ? value.guid : new System.Guid());
            }
        }

        public bool enabled
        {
            get => Bindings.RendererGetActive(handle);
            set => Bindings.RendererSetActive(handle, value);
        }
    }
}
