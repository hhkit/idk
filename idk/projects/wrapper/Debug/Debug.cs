using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class Debug
    {
        /// <summary>
        /// Logs a message to the console.
        /// </summary>
        public static void Log(object message
        ,    [System.Runtime.CompilerServices.CallerMemberName] string memberName = ""
        ,    [System.Runtime.CompilerServices.CallerFilePath] string sourceFilePath = ""
        ,    [System.Runtime.CompilerServices.CallerLineNumber] int sourceLineNumber = 0
            )
        {
            Bindings.DebugLog(sourceFilePath.Substring(sourceFilePath.LastIndexOf('\\') + 1)
                + ":" + sourceLineNumber + ":" + memberName, message.ToString());
        }
    }
}
