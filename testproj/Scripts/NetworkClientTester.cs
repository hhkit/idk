using idk;

namespace TestAndSeek
{
    public class NetworkClientTester
        : MonoBehavior, IConnectionCallbacks
    {
        public int a, b, c, d;

        public void OnConnectedToServer()
        {
            Debug.Log("Connected to server");
        }

        public void OnDisconnectionFromServer()
        {
            Debug.Log("Disconnected to server");
        }

        void Start()
        {
            ElectronNetwork.AddCallbackTarget(this);
        }

        void OnDestroy()
        {
            ElectronNetwork.RemoveCallbackTarget(this);
        }

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.C))
            {
                Address addr = new Address (a,b,c,d);

                ElectronNetwork.Connect(addr);
            }

            if (Input.GetKeyDown(KeyCode.L))
            {
                ElectronNetwork.isListeningForServers = true;
            }

            if (ElectronNetwork.isListeningForServers)
            {
                foreach (var elem in ElectronNetwork.discoveredServers)
                    Debug.Log(elem);
            }

            //if (Input.GetKeyDown(KeyCode.D))
            //{
            //    GetComponent<ElectronView>().RPC("LOL", RPCTarget.All, 0, 5);
            //}
        }
    }
}
