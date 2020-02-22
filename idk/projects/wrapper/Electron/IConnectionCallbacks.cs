using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    /// <summary>
    /// For Client-side, detects when the client connects to the server.
    /// </summary>
    public interface IConnectionCallbacks
    {
        void OnConnectedToServer();
        void OnDisconnectionFromServer();
    }
}
