﻿using idk;

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

        private RigidBody rb;
        private TestShou ts;

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
            rb = gameObject.GetComponent<RigidBody>();
            ts = gameObject.GetComponent<TestShou>();
            if (rb)
                Debug.Log("found rigidbody");

            foreach (var elem in FindObjectsOfType<TestShou>())
            {
                Debug.Log("I " + i + " am " + elem.i);
                Destroy(elem.gameObject);
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
            if (prefab)
                Debug.Log(prefab.ToString());
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
