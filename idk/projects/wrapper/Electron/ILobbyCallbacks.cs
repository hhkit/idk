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
        /// Called when the local player creates a lobby.
        /// </summary>
        void OnLobbyCreated(bool success);

        /// <summary>
        /// Called when the local player joins a lobby.
        /// </summary>
        void OnLobbyJoined(bool success);

        /// <summary>
        /// Called when another member joins the lobby the local player is in.
        /// </summary>
        void OnLobbyMemberJoined(Client client);

        /// <summary>
        /// Called when another member leaves the lobby the local player is in.
        /// </summary>
        void OnLobbyMemberLeft(Client client);

        /// <summary>
        /// Called when lobby metadata changed. Use ElectronNetwork.GetLobbyData() to get data.
        /// </summary>
        void OnLobbyDataUpdated();

        /// <summary>
        /// Called on lobby retrieval from ElectronNetwork.FindLobbies.
        /// </summary>
        void OnLobbyMatchList(Lobby[] lobbies);

        /// <summary>
        /// Gets called every frame for messages received in the lobby, sent by ElectronNetwork.SendLobbyMsg().
        /// Also received by the sender.
        /// </summary>
        void OnLobbyChatMsg(Client sender, byte[] message);

        /// <summary>
        /// Called when the user tries to join a lobby from their friends list.
        /// Game client should attempt to connect to specified lobby when this is received
        /// </summary>
        void OnLobbyJoinRequested(Lobby lobby);
    }
}
