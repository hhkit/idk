using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public interface ILobbyCallbacks
    {
        /// <summary>
        /// Called when THIS player creates a lobby.
        /// </summary>
        void OnLobbyCreated(bool success);

        /// <summary>
        /// Called when THIS player joins a lobby.
        /// </summary>
        void OnLobbyJoined(bool success);

        void OnLobbyMemberJoined(Client client);
        void OnLobbyMemberLeft(Client client);

        /// <summary>
        /// Called when lobby metadata changed. Use ElectronNetwork.GetLobbyData() to get data.
        /// </summary>
        void OnLobbyDataUpdated();

        /// <summary>
        /// Called on lobby retrieval from ElectronNetwork.FindLobbies.
        /// </summary>
        void OnLobbyMatchList(Lobby[] lobbies);

        void OnLobbyChatMsg(Client sender, string message);
    }
}
