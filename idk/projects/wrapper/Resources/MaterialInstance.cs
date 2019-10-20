namespace idk
{
    public class MaterialInstance
        : Resource
    {
        public float GetFloat(string name)
            => Bindings.MaterialInstanceGetFloat(guid, name);
    }
}
