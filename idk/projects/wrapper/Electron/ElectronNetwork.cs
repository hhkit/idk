using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace idk
{
    public static class ElectronNetwork
    {

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
        
        public static bool     isHost      { get => Bindings.NetworkGetIsHost(); }
        public static bool     isConnected { get => Bindings.NetworkGetIsConnected(); }
        public static int      ping        { get => Bindings.NetworkGetPing(); }



        public static void Disconnect() => Bindings.NetworkDisconnect();



        /// <summary>
        /// TODO: lobby type
        /// callback: ILobbyCallbacks.OnLobbyCreated + ILobbyCallbacks.OnLobbyJoined
        /// </summary>
        public static void CreateLobby() => Bindings.NetworkCreateLobby();

        /// <summary>
        /// Joins the specified lobby. Use FindLobbies
        /// callback: ILobbyCallbacks.OnLobbyJoined
        /// </summary>
        public static void JoinLobby(Lobby lobby) => Bindings.NetworkJoinLobby(lobby.id);

        /// <summary>
        /// Leaves the current lobby.
        /// callback for other members: ILobbyCallbacks.OnLobbyMemberLeft
        /// </summary>
        public static void LeaveLobby() => Bindings.NetworkLeaveLobby();

        /// <summary>
        /// Activates the Steam Overlay to open the invite dialog for the current lobby.
        /// </summary>
        public static void OpenLobbyInviteDialog() => Bindings.NetworkOpenLobbyInviteDialog();

        /// <summary>
        /// Requests a list of lobbies.
        /// Callback: ILobbyCallbacks.OnLobbyMatchList
        /// </summary>
        public static void FindLobbies() => Bindings.NetworkFindLobbies();

        /// <summary>
        /// Sets the current lobby data. You can only do this if you're the lobby owner.
        /// </summary>
        public static void SetLobbyData(string key, string value) => Bindings.NetworkSetLobbyData(key, value);

        /// <summary>
        /// Connects P2P to the owner of the current lobby.
        /// see IConnectionCallbacks
        /// </summary>
        public static void ConnectToLobbyOwner() => Bindings.NetworkConnectToLobbyOwner();



        public static void LoadScene(Scene scene)
        {
            if (!isHost)
                throw new InvalidNetworkOperationException("Only the Server can change the scene.");

            Bindings.NetworkLoadScene(scene.guid);
        }
        public static GameObject Instantiate(Prefab prefab, Vector3 position)
        {
            if (!isHost)
                throw new InvalidNetworkOperationException("Only the Host can instantiate prefabs");

            var id = Bindings.NetworkInstantiatePrefabPosition(prefab.guid, position);
            return id != 0 ? new GameObject(id) : null;
        }
        public static GameObject Instantiate(Prefab prefab, Vector3 position, Quaternion rotation)
        {
            if (!isHost)
                throw new InvalidNetworkOperationException("Only the Host can instantiate prefabs");

            var id = Bindings.NetworkInstantiatePrefabPositionRotation(prefab.guid, position, rotation);
            return id != 0 ? new GameObject(id) : null;
        }

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
