using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    /// <summary>
    /// For server-side, detects when players connect to the server.
    /// </summary>
    public interface ILobbyCallbacks
    {
        /// <summary>
        /// Fires event when a single player joins the server
        /// </summary>
        /// <param name="p"></param>
        void OnClientConnect(Client p);
        /// <summary>
        /// Fires event when a player disconnects from the server
        /// </summary>
        /// <param name="p"></param>
        void OnClientDisconnect(Client p);
    }
}
