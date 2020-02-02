namespace idk
{
    public static class ElectronNetwork
    {
        #region Delegates
        public delegate void ClientConnectionSignal(Player id);
        public delegate void ServerConnectionSignal();

        public static ClientConnectionSignal OnClientConnected;
        public static ClientConnectionSignal OnClientDisconnected;

        public static ServerConnectionSignal OnServerConnected;
        public static ServerConnectionSignal OnServerDisconnected;
        #endregion

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
            OnClientConnected(new Player() { ActorNumber = id }) ;
        }

        internal static void ExecClientDisconnect(int id)
        {
            OnClientDisconnected(new Player() { ActorNumber = id });
        }

        internal static void ExecServerConnect()
        {
            OnServerConnected();
        }

        internal static void ExecServerDisconnect()
        {
            OnServerDisconnected();
        }
    }
}
