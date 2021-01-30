using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace idk
{
    public static class ElectronNetwork
    {
        /// <summary>
        /// Get the clients connected to this server (clients must use ElectronNetwork.ConnectToLobbyOwner()).
        /// </summary>
        public static IList<Client> clients 
        { 
            get
            {
                var retval = new List<Client>();
                foreach (var id in Bindings.NetworkGetClients())
                    retval.Add(new Client(id));
                return retval;
            } 
        }
        
        public static bool  isHost          { get => Bindings.NetworkGetIsHost(); }
        public static bool  isConnected     { get => Bindings.NetworkGetIsConnected(); }

        public static int   ping            { get => Bindings.NetworkGetPing(); }
        public static bool  isRollingBack   { get => Bindings.NetworkGetIsRollingBack(); }
        public static float predictionWeight 
        { 
            get => Bindings.NetworkGetPredictionWeight();
            set => Bindings.NetworkSetPredictionWeight(value);
        }

        /// <summary>
        /// Gets the current Lobby, or null.
        /// </summary>
        public static Lobby lobby
        {
            get
            {
                ulong id = Bindings.NetworkGetCurrentLobby();
                return id == 0 ? null : new Lobby(id);
            }
        }

        /// <summary>
        /// Gets the local Client in the current lobby. If not in any lobby, returns null.
        /// </summary>
        public static Client local
        {
            get 
            {
                int id = Bindings.NetworkGetLocalClient(); 
                return id == -2 ? null : new Client(id); 
            }
        }



        /// <summary>
        /// Disconnects from server or clients, if any. Note that this DOES NOT leave the current lobby!
        /// </summary>
        public static void Disconnect() => Bindings.NetworkDisconnect();



        /// <summary>
        /// Callback: ILobbyCallbacks.OnLobbyCreated + ILobbyCallbacks.OnLobbyJoined
        /// </summary>
        public static void CreateLobby(LobbyType lobbyType) => Bindings.NetworkCreateLobby((int)lobbyType);

        /// <summary>
        /// Joins the specified lobby. Use FindLobbies<br />
        /// Callback: ILobbyCallbacks.OnLobbyJoined
        /// </summary>
        public static void JoinLobby(Lobby lobby) => Bindings.NetworkJoinLobby(lobby.id);

        /// <summary>
        /// Leaves the current lobby. If connected, disconnects first.<br />
        /// Callback for other members: ILobbyCallbacks.OnLobbyMemberLeft
        /// </summary>
        public static void LeaveLobby() => Bindings.NetworkLeaveLobby();

        /// <summary>
        /// Activates the Steam Overlay to open the invite dialog for the current lobby.
        /// </summary>
        public static void OpenLobbyInviteDialog() => Bindings.NetworkOpenLobbyInviteDialog();

        /// <summary>
        /// Requests a list of lobbies.<br />
        /// Callback: ILobbyCallbacks.OnLobbyMatchList
        /// </summary>
        public static void FindLobbies() => Bindings.NetworkFindLobbies();

        /// <summary>
        /// Get the member at the specified index of the current lobby. Works for all lobby members.
        /// </summary>
        public static Client GetLobbyMember(int i) => new Client(Bindings.NetworkGetLobbyMember(i));

        /// <summary>
        /// Sets the current lobby data. You can only do this if you're the lobby owner.
        /// </summary>
        public static void SetLobbyData(string key, string value) => Bindings.NetworkSetLobbyData(key, value);

        /// <summary>
        /// Broadcasts a chat message to the all of the users in the lobby.
        /// <para />
        /// All users in the lobby (including the local user) will receive a ILobbyCallbacks.OnLobbyChatMsg callback.
        /// <para />
        /// If you're sending binary data, you should prefix a header to the message
        /// so that you know to treat it as your custom data rather than a plain old text message.
        /// <para />
        /// For communication that needs to be arbitrated (for example having a user pick from a set of characters,
        /// and making sure only one user has picked a character), you can use the lobby owner as the decision maker.
        /// There is guaranteed to always be one and only one lobby member who is the owner.
        /// So for the choose-a-character scenario, the user who is picking a character would send
        /// the binary message 'I want to be Zoe', the lobby owner would see that message, see if it was OK,
        /// and broadcast the appropriate result (user X is Zoe).
        /// <para />
        /// These messages are sent via the Steam back-end, and so the bandwidth available is limited.
        /// For higher-volume traffic like voice or game data, you'll want to use P2P functionality. See ConnectToLobbyOwner.
        /// </summary>
        public static void SendLobbyMsg(byte[] msg) => Bindings.NetworkSendLobbyMsg(msg);

        /// <summary>
        /// Connects P2P to the owner of the current lobby. This is required for P2P functionality and network synchronization.<br />
        /// see IConnectionCallbacks
        /// </summary>
        public static void ConnectToLobbyOwner() => Bindings.NetworkConnectToLobbyOwner();



        /// <summary>
        /// Loads a scene for everyone. Requires connection. Can only be called by the server.
        /// </summary>
        public static void LoadScene(Scene scene)
        {
            if (!isHost)
                throw new InvalidNetworkOperationException("Only the Server can change the scene.");

            Bindings.NetworkLoadScene(scene.guid);
        }

        /// <summary>
        /// Instantiates a networked prefab in the scene.
        /// </summary>
        public static GameObject Instantiate(Prefab prefab, Vector3 position)
        {
            if (!isHost)
                throw new InvalidNetworkOperationException("Only the Host can instantiate prefabs");

            var id = Bindings.NetworkInstantiatePrefabPosition(prefab.guid, position);
            return id != 0 ? new GameObject(id) : null;
        }
        /// <summary>
        /// Instantiates a networked prefab in the scene.
        /// </summary>
        public static GameObject Instantiate(Prefab prefab, Vector3 position, Quaternion rotation)
        {
            if (!isHost)
                throw new InvalidNetworkOperationException("Only the Host can instantiate prefabs");

            var id = Bindings.NetworkInstantiatePrefabPositionRotation(prefab.guid, position, rotation);
            return id != 0 ? new GameObject(id) : null;
        }



        /// <summary>
        /// Add a MonoBehavior with interface ILobbyCallbacks or IConnectionCallbacks so that the callbacks will be fired.
        /// </summary>
        public static void AddCallbackTarget(MonoBehavior target)
        {
            Bindings.NetworkAddCallback(target.handle);
        }
        public static void RemoveCallbackTarget(MonoBehavior target)
        {
            Bindings.NetworkRemoveCallback(target.handle);
        }


        internal static byte[] Serialize(object serialize_me)
        {
            var formatter = new BinaryFormatter();
            using (MemoryStream stream = new MemoryStream())
            {
                formatter.Serialize(stream, serialize_me);
                return stream.ToArray();
            }
        }

        internal static object Reserialize(byte[] bytes)
        {
            var formatter = new BinaryFormatter();
            using (MemoryStream stream = new MemoryStream(bytes))
            {
                return formatter.Deserialize(stream);
            }
        }
    }
}
