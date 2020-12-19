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

        private static int totalTextures = 0;
        public static float loadPercent
        {
            get
            {
                int pendingTextures = Bindings.GraphicsGetVarInt("pending_textures");
                totalTextures = pendingTextures > totalTextures ? pendingTextures : totalTextures;
                int loadedTextures = totalTextures - pendingTextures;

                int totalModels = Bindings.GraphicsGetVarInt("total_async_resources");
                int loadedModels = Bindings.GraphicsGetVarInt("async_resources_loaded");

                int total = totalTextures + totalModels;
                int loaded = loadedTextures + loadedModels;

                if (total == 0 || loaded == 0)
                    return 0;
                else
                    return (float)loaded / total;
            }
        }

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
