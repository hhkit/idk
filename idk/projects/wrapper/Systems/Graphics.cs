namespace idk
{
    public static class Graphics
    {
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
