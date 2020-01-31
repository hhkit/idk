using idk;

namespace TestAndSeek
{
    public class NetworkHostTester
        : MonoBehavior
    {
        public Scene scene;

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.H))
                ElectronNetwork.CreateLobby();

            if (Input.GetKeyDown(KeyCode.E))
                ElectronNetwork.LoadScene(scene);
        }
    }
}
