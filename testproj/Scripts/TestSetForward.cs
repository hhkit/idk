using idk;

namespace TestAndSeek
{
    public class TestSetForward
        : MonoBehavior
    {
        void Start()
        {
        }


        void FixedUpdate()
        {

        }

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.I))
                transform.forward = Vector3.forward;

            if (Input.GetKeyDown(KeyCode.J))
                transform.forward = Vector3.left;

            if (Input.GetKeyDown(KeyCode.L))
                transform.forward = Vector3.right;

            if (Input.GetKeyDown(KeyCode.K))
                transform.forward = Vector3.back;

            Quaternion q = new Quaternion();

            if (Input.GetKeyDown(KeyCode.U))
            {
                q.eulerAngles = new Vector3(0, 90, 0);
                transform.forward = q*transform.forward;
            }

            if (Input.GetKeyDown(KeyCode.E))
            {
                q.eulerAngles = new Vector3(0, -90, 0);
                transform.forward = q * transform.forward;
            }

            if (Input.GetKeyDown(KeyCode.X))
            {
                q.eulerAngles = new Vector3(0, 180, 0);
                transform.forward = q* transform.forward;
            }
        }
    };
}