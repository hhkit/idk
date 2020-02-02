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
            if (Input.GetKey(KeyCode.I))
            {
                Debug.Log("move fwd");
                obj.GetComponent<RigidBody>().AddForce(obj.transform.forward * movespeed);
            }
            if (Input.GetKey(KeyCode.K))
            {
                Debug.Log("jmp");
                obj.GetComponent<RigidBody>().AddForce(Vector3.up * 25f);
            }
            if (Input.GetKey(KeyCode.J))
                obj.transform.rotation = Quaternion.AngleAxis(turnspeed * Time.deltaTime, Vector3.up) * obj.transform.rotation;

            if (Input.GetKey(KeyCode.L))
                obj.transform.rotation = Quaternion.AngleAxis(-turnspeed * Time.deltaTime, Vector3.up) * obj.transform.rotation;

            if (Input.GetKeyDown(KeyCode.H))
                ElectronNetwork.CreateLobby();

            if (Input.GetKeyDown(KeyCode.S))
                ElectronNetwork.LoadScene(scene);

            if (Input.GetKeyDown(KeyCode.V))
                obj.GetComponent<RigidBody>().velocity = new Vector3(2,0,0);

            if (Input.GetKeyDown(KeyCode.P))
                obj = ElectronNetwork.Instantiate(instantiate, transform.position);

            if (Input.GetKeyDown(KeyCode.T))
                obj.GetComponent<ElectronView>().TransferOwnership(FindObjectOfType<NetworkDebugger>().player);

            if (Input.GetKeyDown(KeyCode.R))
            {
                obj?.GetComponent<ElectronView>().RPC("ExecuteMeWithArgs", RPCTarget.All, "hello", 5, "oh no");
            }
        }
    }
}
