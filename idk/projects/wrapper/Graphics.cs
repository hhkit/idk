namespace idk
{
    public class Graphics
    {
        public bool use_gamma_correction
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
