using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class NullRef
        : Exception
    {
        public NullRef()
            : base("Null Object Exception")
        { }
    }
}
