namespace idk
{
    public class Graphics
    {
        public bool useGammaCorrection
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
