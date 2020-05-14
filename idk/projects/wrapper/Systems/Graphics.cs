namespace idk
{
    public static class Graphics
    {
        public enum Quality
        {
            eLow,
            eHigh
        }

        public static void SetGraphicsQuality(Quality quality)
        {
            switch(quality)
            {
                case Quality.eHigh:
                Bindings.GraphicsSetVarBool("HalfMip",false);
                break;
                case Quality.eLow:
                Bindings.GraphicsSetVarBool("HalfMip",true);
                break;
            }
            Bindings.GraphicsSetVarBool("Reload Textures",true);
        }
        public static Quality GetGraphicsQuality()
        {
            return Bindings.GraphicsGetVarBool("HalfMip")?Quality.eLow:Quality.eHigh;
        }

        public static float gammaCorrection
        {
            get
            {
                return Bindings.GraphicsGetGammaCorrection();
            }
            set
            {
                Bindings.GraphicsSetGammaCorrection(value);
            }
        }
        public static void SetGammaCorrection(float gamma_value)
        {
            Bindings.GraphicsSetGammaCorrection(gamma_value);
        }
        public static void DisableGammaCorrection()
        {
            Bindings.GraphicsDisableGammaCorrection();
        }
    }
}
