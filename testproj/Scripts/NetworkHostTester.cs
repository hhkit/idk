using idk;

namespace TestAndSeek
{
    public class NetworkHostTester
        : MonoBehavior
    {
        public Scene scene;
        public Prefab instantiate;
        public GameObject obj;

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.H))
                ElectronNetwork.CreateLobby();

            if (Input.GetKeyDown(KeyCode.S))
                ElectronNetwork.LoadScene(scene);

            if (Input.GetKeyDown(KeyCode.V))
                obj.GetComponent<RigidBody>().velocity = new Vector3(2,0,0);

            if (Input.GetKeyDown(KeyCode.P))
                obj = ElectronNetwork.Instantiate(instantiate, transform.position);

            if (Input.GetKeyDown(KeyCode.T))
                obj.GetComponent<ElectronView>().TransferOwnership(0);

            if (Input.GetKeyDown(KeyCode.R))
            {
                obj?.GetComponent<ElectronView>().RPC("ExecuteMeWithArgs", RPCTarget.All, "hello", 5, "oh no");
            }
        }
    }
}
