using System.Collections;
using System.Collections.Generic;

namespace idk
{
    public static class ElectronNetwork
    {

        public static IList<Player> players 
        { get
            {
                var retval = new List<Player>();
                foreach (var id in Bindings.NetworkGetPlayers())
                    retval.Add(new Player(id));
                return retval;
            } 
        }

        public static bool isHost { get => Bindings.NetworkGetIsHost(); }
        public static bool isConnected { get => Bindings.NetworkGetIsConnected(); }
        public static int ping { get => Bindings.NetworkGetPing(); }
        public static Device[] devices { get => Bindings.NetworkGetDevices(); }

        public static void Disconnect() => Bindings.NetworkDisconnect();
        public static void CreateLobby() => Bindings.NetworkCreateLobby();
        public static void Connect(Address a) => Bindings.NetworkConnect(a);
        public static void LoadScene(Scene scene) => Bindings.NetworkLoadScene(scene.guid);
        public static GameObject Instantiate(Prefab prefab, Vector3 position)
        {
            if (!isHost)
                return null;

            var id = Bindings.NetworkInstantiatePrefabPosition(prefab.guid, position);
            return id != 0 ? new GameObject(id) : null;
        }
        public static GameObject Instantiate(Prefab prefab, Vector3 position, Quaternion rotation)
        {
            if (!isHost)
                return null;

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
    }
}
