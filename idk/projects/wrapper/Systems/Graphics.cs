namespace idk
{
    public static class Graphics
    {
        public static bool useGammaCorrection
        {
            get
            {
                return Bindings.GraphicsGetSRGB();
            }
            set
            {
                Bindings.GraphicsSetSRGB(value);
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
