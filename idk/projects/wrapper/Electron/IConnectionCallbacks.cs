using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public interface IConnectionCallbacks
    {
        void OnConnectedToServer();
        void OnDisconnectedFromServer();
        void OnClientConnected(Client client);
        void OnClientDisconnected(Client client);
    }
}
