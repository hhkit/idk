using System;

namespace idk
{
    class IDK
    {
        public static void PrintException(Exception e)
        {
            Bindings.DebugLog("Exception", e.ToString());
        }

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
