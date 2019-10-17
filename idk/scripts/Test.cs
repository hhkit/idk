using idk;

namespace TestAndSeek
{
    public class Test 
        : MonoBehavior
    {
        public Test()
        {
            System.Console.WriteLine("Oh no");
        }
        public void Thunderbolt(Vector3 v)
        {
            System.Console.WriteLine("pikachu use");
            System.Console.WriteLine("thunderbolt {0} {1} {2}", v.x, v.y, v.z);
        }
        public override void Update()
        {
            System.Console.WriteLine("Poop.");
            //transform.position = transform.position + Vector3.up * 0.016f;
        }

        public void TestTransform(Transform t)
        {
            System.Console.WriteLine("TestTransform");
            Thunderbolt(t.position);
            t.position = new Vector3(5, 7, 1);
            Thunderbolt(t.position);
            t.position.Normalize();
            Thunderbolt(t.position);
            t.gameObject.SetActive(false);
            t.gameObject.transform.position = new Vector3(5, 4, 3);
            Thunderbolt(t.gameObject.transform.position);
        }

        public static void Main(string[] args)
        {
            System.Console.WriteLine("Yolo");
        }
    }
}
