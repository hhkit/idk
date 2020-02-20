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

        [ElecRPC]
        void ExecuteMe()
        {
            Debug.Log("RPC executed");
        }
        [ElecRPC]
        void ExecuteMeWithArgs(string param, int val, string param2)
        {
            Debug.Log("RPC executed " + param + "," + val + "," + param2);
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

            if (Input.GetKeyDown(KeyCode.D))
            {
                GetComponent<ElectronView>().RPC("LOL", RPCTarget.All, 0, 5);
            }
        }
    }
}
