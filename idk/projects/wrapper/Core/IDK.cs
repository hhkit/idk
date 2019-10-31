using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    class IDK
    {
        public static bool TypeIsAbstract(Type t)
        {
            return t != null ? t.IsAbstract : true;
        }
        public static void Main(string[] args)
        {
            Console.WriteLine("loaded");
        }
    }
}
