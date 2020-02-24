using idk;

namespace TestAndSeek
{
    public class NetworkHostTester
        : MonoBehavior, ILobbyCallbacks
    {
        public Scene scene;
        public Prefab instantiate;
        public GameObject obj;
        public float turnspeed = 90;
        public float movespeed = 35;

        public void OnClientConnect(Client p)
        {
            Debug.Log("Player " + p.ActorNumber + " connected");
        }

        public void OnClientDisconnect(Client p)
        {
            Debug.Log("Player " + p.ActorNumber + " disconnected");
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
            if (Input.GetKey(KeyCode.Shift))
            {
                if (Input.GetKeyDown(KeyCode.H))
                    ElectronNetwork.CreateLobby();

                if (Input.GetKeyDown(KeyCode.S))
                    ElectronNetwork.LoadScene(scene);

                if (Input.GetKeyDown(KeyCode.P))
                {
                    obj = ElectronNetwork.Instantiate(instantiate, transform.position);
                    obj.GetComponent<ElectronView>().TransferOwnership(ElectronNetwork.clients[0]);
                }
                if (Input.GetKeyDown(KeyCode.R))
                    obj?.GetComponent<ElectronView>().RPC("ExecuteMeWithArgs", RPCTarget.All, "hello", 5, "oh no");

                if (Input.GetKeyDown(KeyCode.R))
                    obj?.GetComponent<ElectronView>().RPC("ExecuteMeCheckingForSender", RPCTarget.All, "oh dear");

                if (Input.GetKeyDown(KeyCode.T))
                    obj?.GetComponent<ElectronView>().TransferOwnership(ElectronNetwork.clients[0]);
            }
        }
    }
}
