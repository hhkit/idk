using System.Collections;
using System.Collections.Generic;

namespace idk
{
    public static class ElectronNetwork
    {
        #region Delegates
        public delegate void ClientConnectionSignal(Player id);
        public delegate void ServerConnectionSignal();

        public static event ClientConnectionSignal OnClientConnected;
        public static event ClientConnectionSignal OnClientDisconnected;

        public static event ServerConnectionSignal OnServerConnected;
        public static event ServerConnectionSignal OnServerDisconnected;
        #endregion

        static List<Player> players = new List<Player>();

        public static IList<Player> GetPlayers()
        {
            return players;
        }

        public static bool IsHost { get => Bindings.NetworkGetIsHost(); }
        public static bool IsConnected { get => Bindings.NetworkGetIsConnected(); }
        public static int GetPing { get => Bindings.NetworkGetPing(); }
        public static void Disconnect() => Bindings.NetworkDisconnect();
        public static void CreateLobby() => Bindings.NetworkCreateLobby();
        public static void Connect(Address a) => Bindings.NetworkConnect(a);
        public static void LoadScene(Scene scene) => Bindings.NetworkLoadScene(scene.guid);
        public static GameObject Instantiate(Prefab prefab, Vector3 position)
        {
            if (!IsHost)
                return null;

            var id = Bindings.NetworkInstantiatePrefabPosition(prefab.guid, position);
            return id != 0 ? new GameObject(id) : null;
        }
        public static GameObject Instantiate(Prefab prefab, Vector3 position, Quaternion rotation)
        {
            if (!IsHost)
                return null;

            var id = Bindings.NetworkInstantiatePrefabPositionRotation(prefab.guid, position, rotation);
            return id != 0 ? new GameObject(id) : null;
        }

        internal static void ExecClientConnect(int id)
        {
            //Player p = new Player(id);
            //players.Add(p);
            //OnClientConnected(p) ;
        }

        internal static void ExecClientDisconnect(int id)
        {
            /*
            foreach (var p in players)
                if (p.ActorNumber == id)
                {
                    players.Remove(p);
                    //OnClientDisconnected(p);
                    break;
                }
                */
        }

        internal static void ExecServerConnect()
        {
            //OnServerConnected();
        }

        internal static void ExecServerDisconnect()
        {
        //    OnServerDisconnected();
        }

        internal static void Initialize()
        {
            players = new List<Player>();
            foreach (var id in Bindings.NetworkGetPlayers())
                players.Add(new Player(id));
        }
    }
}
