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
    }
}
