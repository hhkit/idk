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
        }

        public void TestTransform(Transform t)
        {
            System.Console.WriteLine("TestTransform");
            Thunderbolt(t.position);
        }

        public static void Main(string[] args)
        {
            System.Console.WriteLine("Yolo");
        }
    }
}
