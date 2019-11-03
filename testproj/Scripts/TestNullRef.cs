using idk;

namespace TestAndSeek
{
    public class TestNullRef
        : MonoBehavior
    {
        RigidBody rb;

        void Start()
        {
            rb = GetComponent<RigidBody>();
        }

        void Update()
        {
            //if ((object)rb == null)
            //    throw new System.NullReferenceException();

            rb.velocity = new Vector3(10, 0, 0);
        }
    }
}
