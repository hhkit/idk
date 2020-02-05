using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    class Graphics
    {
        public bool DefaultRenderTargetSrgb 
        {
            get
            {
                return Bindings.DefRtSrgb();
            }
            set
            {
                Bindings.ToggleDefRtSrgb(value);
            }
        }
    }
}
