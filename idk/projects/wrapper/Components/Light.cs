
namespace idk
{
    public class Light
       : Component
    {
        public bool enabled
        {
            get => Bindings.LightGetEnabled(handle);
            set => Bindings.LightSetEnabled(handle, value);
        }

        public Color color
        {
            get => Bindings.LightGetColor(handle);
            set => Bindings.LightSetColor(handle, value);
        }

        public float intensity
        {
            get => Bindings.LightGetIntensity(handle);
            set => Bindings.LightSetIntensity(handle, value);
        }

        public bool castsShadows
        {
            get => Bindings.LightGetCastShadow(handle);
            set => Bindings.LightSetCastShadow(handle, value);
        }

        public float shadowBias
        {
            get => Bindings.LightGetShadowBias(handle);
            set => Bindings.LightSetShadowBias(handle, value);
        }

        /// <summary>
        /// Gets the attenuation radius, ie. range of the spotlight / pointlight. 
        /// For directional light, returns 0.
        /// </summary>
        public float attenuationRadius
        {
            get => Bindings.LightGetAttenuationRadius(handle);
            set => Bindings.LightSetAttenuationRadius(handle, value);
        }

        /// <summary>
        /// Whether light falls off using the inverse square law (intensity = 1 / distance^2)
        /// ie. light gets dimmer quickly as it moves away from the source.
        /// Only for spotlight and pointlight, returns false for directional light.
        /// </summary>
        public bool useInverseSquareAttenuation
        {
            get => Bindings.LightGetIsInverseSqAtt(handle);
            set => Bindings.LightSetIsInverseSqAtt(handle, value);
        }

        public float innerSpotAngle
        {
            get => Bindings.LightGetInnerSpotAngle(handle);
            set => Bindings.LightSetInnerSpotAngle(handle, value);
        }

        public float outerSpotAngle
        {
            get => Bindings.LightGetOuterSpotAngle(handle);
            set => Bindings.LightSetOuterSpotAngle(handle, value);
        }
    }
}
