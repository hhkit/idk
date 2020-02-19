using System;

namespace idk
{
    public class IDK
    {
        static void PrintException(Exception e)
        {
            Bindings.DebugLog("Exception", e.ToString());
        }

        static bool TypeIsAbstract(Type t)
        {
            return t != null ? t.IsAbstract : true;
        }

        static void Main(string[] args)
        {
            Console.WriteLine("loaded IDK lib");
        }
    }
}
