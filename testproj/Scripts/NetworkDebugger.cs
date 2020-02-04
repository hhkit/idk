using idk;

namespace TestAndSeek
{
    class NetworkDbugger
        : MonoBehavior
    {
        public Player player;
        void Start()
        {
            ElectronNetwork.OnClientConnected += (Player p) => { player = p; Debug.Log("Player " + p.ActorNumber + " connected!"); };
            ElectronNetwork.OnClientDisconnected += (Player p) => { player = null;  Debug.Log("Player " + p.ActorNumber + " disconnected!"); };
            ElectronNetwork.OnServerConnected += () => { Debug.Log("connected to server!"); };
            ElectronNetwork.OnServerDisconnected += () => { Debug.Log("disconnected from server!"); };
        }
    }
}
