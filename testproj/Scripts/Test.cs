using idk;
using System.Collections.Generic;

namespace TestAndSeek
{
    public class Test 
        : MonoBehavior
    {
        public int i;
        public float f;
        public float jump_force;
        public Vector3 movement;
        
        public Prefab prefab;
        public GameObject go;
        public MaterialInstance minst;
        public string nama;
        public string namae_o;

        public int i2;

        private RigidBody rb;
        private TestShou ts;

        List<Prefab> pfb;

        static int static_i = 0;

        public Test()
        {
            Debug.Log("Test is constructed");
        }
        public void Thunderbolt(Vector3 v)
        {
            Debug.Log("Pikachu, use");
            //Debug.Log("Thunderbolt! {0} {1} {2}", v.x, v.y, v.z);
            System.Console.WriteLine("and takedown {0}", i);
        }

        void Start()
        {
            pfb.Add(prefab);
            rb = gameObject.GetComponent<RigidBody>();
            ts = gameObject.GetComponent<TestShou>();
            Debug.Log("static_i " + ++static_i);
            if (rb)
                Debug.Log("found rigidbody");

            if (go)
                Debug.Log("found gameobject" + go.name);

            foreach (var elem in FindObjectsOfType<TestShou>())
            {
                Debug.Log("I found a TestShou with player index " + elem.playerIndex);
                //Destroy(elem.gameObject);
            }
        }

        void OnTriggerEnter(Collider other)
        {
            f = 500;
        }

        void OnTriggerStay(Collider other)
        {
            transform.rotation = Quaternion.Euler(0, 180 * Time.fixedDeltaTime, 0) * transform.rotation ;
            f += 5 * Time.fixedDeltaTime;
            Debug.Log("STAY" + f);
        }

        void OnTriggerExit(Collider other)
        {
            i += 1;
        }

        void FixedUpdate()
        {
            //Debug.Log("static_i " + ++static_i);

            if (Input.GetKeyDown(KeyCode.I))
            {
                Debug.Log("spawning prefab");
                var inst = prefab.Instantiate();
                Debug.Log("spawned with " + inst.transform.GetChildren().Length + " children.");
                //Debug.Log("minst:" + minst);
                //Debug.Log("rend: " + gameObject.GetComponentInChildren<Renderer>());
                //gameObject.GetComponentInChildren<Renderer>().materialInstance = minst;
            }

            if (Input.GetKeyDown(KeyCode.P))
                Time.timeScale = 0f;

            if (Input.GetKeyDown(KeyCode.U))
                Time.timeScale = 1f;

            if (rb)
            {
                System.Console.WriteLine("h: {0}, v: {1}", Input.GetAxis(Axis.Horizontal), Input.GetAxis(Axis.Vertical));
                rb.AddForce(f * (Input.GetAxis(Axis.Horizontal) * Vector3.right + Input.GetAxis(Axis.Vertical) * Vector3.forward));


                if (Input.GetKey(KeyCode.W)) rb.AddForce(f * Vector3.forward);
                if (Input.GetKey(KeyCode.S)) rb.AddForce(f * Vector3.back);
                if (Input.GetKey(KeyCode.A)) rb.AddForce(f * Vector3.left);
                if (Input.GetKey(KeyCode.D)) rb.AddForce(f * Vector3.right);
                if (Input.GetKeyDown(KeyCode.I))
                {
                    Debug.Log("Try instantiate");
                    if (prefab)
                    {
                        Debug.Log("instantiating");
                        prefab.Instantiate(transform.position + Vector3.up);
                    }
                    else
                        Debug.Log("INSTANTIATION");
                }
                if (Input.GetKeyDown(KeyCode.Space) || Input.GetKeyDown(KeyCode.JoystickButtonA))
                    rb.AddForce(jump_force * Vector3.up);
            }
            if (ts)
            {
                ts.PrintI();
            }
        }

        void Update()
        {
            //Debug.Log("Children" + transform.GetChildren().Length.ToString());
            //if (prefab)
            //    Debug.Log(prefab.ToString());

            //if (Input.GetKeyDown(KeyCode.L))
            //    GetComponentInParent<TestShou>().PrintI();

            if (Input.GetKeyDown(KeyCode.R))
                transform.forward = Quaternion.AngleAxis(-3.14f / 2, Vector3.up) * transform.forward;

            if (Input.GetKey(KeyCode.Z))
                transform.forward = Quaternion.AngleAxis(-3.14f / 6 * Time.deltaTime, Vector3.up) * transform.forward;

            if (Input.GetKey(KeyCode.X))
                transform.forward = Quaternion.AngleAxis(+3.14f / 6 * Time.deltaTime, Vector3.up) * transform.forward;

        }

        void PausedUpdate()
        {
            Debug.Log("oh boi");
        }

        public void OnCollisionEnter(Collision c)
        {
            Debug.Log("Collide with " + c.gameObject.name +" with normal " + c.normal.x + " " + c.normal.y + " " + c.normal.z);
        }

        public void TestTransform(Transform t)
        {
            System.Console.WriteLine("TestTransform");
            //Thunderbolt(t.position);
            //t.position = new Vector3(5, 7, 1);
            //Thunderbolt(t.position);
            //t.position.Normalize();
            //Thunderbolt(t.position);
            //t.gameObject.SetActive(false);
            //t.gameObject.transform.position = new Vector3(5, 4, 3);
            Thunderbolt(t.gameObject.transform.position);
            System.Console.WriteLine("Hello!");

            //var go = GameObject.FindWithTag("futanari");
            //if (go != null)
            //{
            //    var metallic = go.GetComponent<MeshRenderer>().materialInstance.GetFloat("Metallic");
            //    System.Console.WriteLine("{0} tagged futanari, metallic value {1}", go.name, metallic);
            //}
            System.Console.WriteLine("Out of PP!");
        }

        public static void Main(string[] args)
        {
            System.Console.WriteLine("Yolo");
        }
    }
}
