namespace idk
{
    public static class Graphics
    {
        public enum Quality
        {
            Low,
            High
        }

        public static Quality quality
        {
            get
            {
                return Bindings.GraphicsGetVarBool("HalfMip") ? Quality.Low : Quality.High;
            }
            set
            {
                switch (value)
                {
                    case Quality.High:
                        Bindings.GraphicsSetVarBool("HalfMip", false);
                        break;
                    case Quality.Low:
                        Bindings.GraphicsSetVarBool("HalfMip", true);
                        break;
                }
                Bindings.GraphicsSetVarBool("Reload Textures", true);
            }
        }

        public static int pendingTexturesCount => Bindings.GraphicsGetVarInt("pending_textures");

        public static float gammaCorrection
        {
            get => Bindings.GraphicsGetGammaCorrection();
            set => Bindings.GraphicsSetGammaCorrection(value);
        }

        public static void DisableGammaCorrection()
        {
            Bindings.GraphicsDisableGammaCorrection();
        }
    }
}
