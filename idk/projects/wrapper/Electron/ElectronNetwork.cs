namespace idk
{
    public class ElectronNetwork
    {
        public static bool IsHost { get => Bindings.NetworkGetIsHost(); }
        public static bool IsConnected { get => Bindings.NetworkGetIsConnected(); }
        public static int GetPing { get => Bindings.NetworkGetPing(); }
        public static void Disconnect() => Bindings.NetworkDisconnect();
        public static void CreateLobby() => Bindings.NetworkCreateLobby();
        public static void Connect(Address a) => Bindings.NetworkConnect(a);
        public static void LoadScene(Scene scene) => Bindings.NetworkLoadScene(scene);
        public GameObject Instantiate(Prefab prefab, Vector3 position)
        {
            var id = Bindings.NetworkInstantiatePrefabPosition(prefab.guid, position);
            return id != 0 ? new GameObject(id) : null;
        }
        public GameObject Instantiate(Prefab prefab, Vector3 position, Quaternion rotation)
        {
            var id = Bindings.NetworkInstantiatePrefabPositionRotation(prefab.guid, position, rotation);
            return id != 0 ? new GameObject(id) : null;
        }
    }
}
