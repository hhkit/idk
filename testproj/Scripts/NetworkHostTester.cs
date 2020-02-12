using idk;

namespace TestAndSeek
{
    public class NetworkHostTester
        : MonoBehavior
    {
        public Scene scene;
        public Prefab instantiate;
        public GameObject obj;
        public float turnspeed = 90;
        public float movespeed = 35;

        void Update()
        {
            if (Input.GetKey(KeyCode.Shift))
            {
                if (Input.GetKeyDown(KeyCode.H))
                    ElectronNetwork.CreateLobby();

                if (Input.GetKeyDown(KeyCode.S))
                    ElectronNetwork.LoadScene(scene);

                if (Input.GetKeyDown(KeyCode.P))
                    obj = ElectronNetwork.Instantiate(instantiate, transform.position);

                if (Input.GetKeyDown(KeyCode.R))
                    obj?.GetComponent<ElectronView>().RPC("ExecuteMeWithArgs", RPCTarget.All, "hello", 5, "oh no");

                if (Input.GetKeyDown(KeyCode.T))
                    obj?.GetComponent<ElectronView>().TransferOwnership(ElectronNetwork.GetPlayers()[0]);
            }
        }
    }
}
