using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class InvalidNetworkOperationException
        : System.Exception
    {
        private String error;
        public InvalidNetworkOperationException(String reason)
        {
            error = reason;
        }
        public override string ToString()
        {
            return base.ToString() + error;
        }
    }
}
