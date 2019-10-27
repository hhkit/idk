using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class Debug
    {
        public static void Log(string log
        ,   [System.Runtime.CompilerServices.CallerMemberName] string memberName = ""
        ,    [System.Runtime.CompilerServices.CallerFilePath] string sourceFilePath = ""
        ,    [System.Runtime.CompilerServices.CallerLineNumber] int sourceLineNumber = 0
            )
        {
            Bindings.DebugLog(sourceFilePath.Substring(sourceFilePath.LastIndexOf('\\') + 1)
                + ":" + sourceLineNumber + ":" + memberName, log);
        }
    }
}
