using idk;

namespace TestAndSeek
{
    public class NetworkClientTester
        : MonoBehavior
    {
        public int a, b, c, d;
        void Update()
        {
            if (Input.GetKeyDown(KeyCode.C))
            {
                Address addr = new Address (a,b,c,d);

                ElectronNetwork.Connect(addr);
            }
        }
    }
}
