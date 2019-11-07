
namespace idk
{
    public class Light
       : Component
    {
        public Color Color
        {
            get => Bindings.LightGetColor(handle);
            set => Bindings.LightSetColor(handle, value);
        }

        public float Intensity
        {
            get => Bindings.LightGetIntensity(handle);
            set => Bindings.LightSetIntensity(handle, value);
        }
    }
}
