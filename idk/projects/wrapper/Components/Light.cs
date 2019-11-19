
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

        public bool IsCastShadow
        {
            get => Bindings.LightGetCastShadow(handle);
            set => Bindings.LightSetCastShadow(handle, value);
        }

        public float ShadowBias
        {
            get => Bindings.LightGetShadowBias(handle);
            set => Bindings.LightSetShadowBias(handle, value);
        }

        public Rad FOV
        {
            get => Bindings.LightGetFOV(handle);
            set => Bindings.LightSetFOV(handle, value);
        }

        public float AttenuationRadius
        {
            get => Bindings.LightGetAttenuationRadius(handle);
            set => Bindings.LightSetAttenuationRadius(handle, value);
        }

        public bool IsInverseSquareAttenuation
        {
            get => Bindings.LightGetIsInverseSqAtt(handle);
            set => Bindings.LightSetIsInverseSqAtt(handle, value);
        }
    }
}
