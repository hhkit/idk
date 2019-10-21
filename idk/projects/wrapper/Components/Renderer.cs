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
        }

    }
}
