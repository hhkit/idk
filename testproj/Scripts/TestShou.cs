using idk;

namespace TestAndSeek
{
    public class TestShou
        : MonoBehavior
    {
        public int playerIndex;
        public float f;
        public Prefab p;
        RigidBody rb;

        public void PrintI()
        {
            Debug.Log("Eye Eye captain: " + playerIndex);
            var t = GetComponentInChildren<Test>();
            if (t)
                Debug.Log("I can't hear you: " + t.i);
        }

        void Start()
        {
            rb = GetComponent<RigidBody>();
        }

        void FixedUpdate()
        {
            System.Console.WriteLine("h: {0}, v: {1}", Input.GetAxis(playerIndex, Axis.Horizontal), Input.GetAxis(playerIndex, Axis.Vertical));
            if (rb)
            {
                Debug.Log("RB PRINT");
                rb.AddForce(f * (Input.GetAxis(playerIndex, Axis.Horizontal) * Vector3.right + Input.GetAxis(playerIndex, Axis.Vertical) * Vector3.forward));
            }
        }

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.I) && p)
                p.Instantiate(gameObject);
        }
    }
}
