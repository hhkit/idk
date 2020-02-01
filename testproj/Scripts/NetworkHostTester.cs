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

            if (Input.GetKeyDown(KeyCode.E))
                ElectronNetwork.LoadScene(scene);

            if (Input.GetKeyDown(KeyCode.P))
                obj = ElectronNetwork.Instantiate(instantiate, transform.position);

            if (Input.GetKeyDown(KeyCode.R))
            {
                obj?.GetComponent<ElectronView>().RPC("ExecuteMeWithArgs", RPCTarget.All, "hello", 5, "oh no");
            }
        }
    }
}
