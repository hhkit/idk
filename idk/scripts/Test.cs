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
            //System.Console.WriteLine("thunderbolt");
            System.Console.WriteLine("thunderbolt {0} {1} {2}", v.x, v.y, v.z);
        }
        public override void Update()
        {
            System.Console.WriteLine("Poop.");
        }
        public static void Main(string[] args)
        {
            System.Console.WriteLine("Yolo");
        }
    }
}
