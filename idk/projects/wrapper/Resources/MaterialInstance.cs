namespace idk
{
    public class MaterialInstance
        : Resource
    {
        public float GetFloat(string name)
            => Bindings.MaterialInstanceGetFloat(guid, name);
        public Vector2 GetVector2(string name)
            => Bindings.MaterialInstanceGetVector2(guid, name);
        public Vector3 GetVector3(string name)
            => Bindings.MaterialInstanceGetVector3(guid, name);
        public Vector4 GetVector4(string name)
            => Bindings.MaterialInstanceGetVector4(guid, name);
        public Texture GetTexture(string name)
        {
            var tex_guid = Bindings.MaterialInstanceGetTexture(guid, name);
            Texture t = new Texture();
            t.guid = tex_guid;
            return t;
        }

        public void SetFloat(string name, float value)
            => Bindings.MaterialInstanceSetFloat(guid, name, value);
        public void SetVector2(string name, Vector2 value)
            => Bindings.MaterialInstanceSetVector2(guid, name, value);
        public void SetVector3(string name, Vector3 value)
            => Bindings.MaterialInstanceSetVector3(guid, name, value);
        public void SetVector4(string name, Vector4 value)
            => Bindings.MaterialInstanceSetVector4(guid, name, value);
        public void SetTexture(string name, Texture tex)
            => Bindings.MaterialInstanceSetTexture(guid, name, tex.guid);
    }
}
