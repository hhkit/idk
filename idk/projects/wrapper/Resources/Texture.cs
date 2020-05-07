namespace idk
{
    public class Texture
        : Resource
    {
        public Vector2 size => Bindings.TextureGetSize(guid);
    }
}