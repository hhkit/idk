using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    /// <summary>
    /// This class adds the property electronView, a property for convenience.
    /// </summary>
    public class MonoBehaviorElec
        : MonoBehavior
    {
        private ElectronView cachedElectronView;
        public ElectronView electronView
        {
            get
            {
                if (!cachedElectronView)
                    cachedElectronView = GetComponent<ElectronView>();
                
                return cachedElectronView;
            }
        }
    }
}
