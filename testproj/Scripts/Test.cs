using idk;

namespace TestAndSeek
{
    public class Test 
        : MonoBehavior
    {
        public int i;
        public float f;
        public Vector3 movement;
        public GameObject go;

        public Test()
        {
            System.Console.WriteLine("Oh no");
        }
        public void Thunderbolt(Vector3 v)
        {
            System.Console.WriteLine("pikachu use");
            System.Console.WriteLine("thunderbolt {0} {1} {2}", v.x, v.y, v.z);
           // System.Console.WriteLine("and takedown {0}", i);
        }
        public override void Update()
        {
            //System.Console.WriteLine("Poop.");
            if (Input.GetKey(KeyCode.W)) transform.position = transform.position + 0.016f * f * Vector3.forward;
            if (Input.GetKey(KeyCode.S)) transform.position = transform.position + 0.016f * f * Vector3.back;
            if (Input.GetKey(KeyCode.A)) transform.position = transform.position + 0.016f * f * Vector3.left;
            if (Input.GetKey(KeyCode.D)) transform.position = transform.position + 0.016f * f * Vector3.right;
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
