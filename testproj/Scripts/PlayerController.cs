using idk;

namespace TestAndSeek
{
    class PlayerController
        : MonoBehavior
    {
        public float jump_force = 25f;
        public float move_speed = 45f;

        public float air_turn_control = 0.02f;
        public float air_move_control = 0.25f;

        public float rot_speed = 90f;

        bool on_floor = false;

        RigidBody rb;
        ElectronView ev;
        void Start()
        {
            rb = GetComponent<RigidBody>();
            ev = GetComponent<ElectronView>();
        }

        void OnTriggerEnter(Collider other)
        {
            if (other.tag == "Floor")
                on_floor = true;
        }
        void OnTriggerExit(Collider other)
        {
            if (other.tag == "Floor")
                on_floor = false;
        }
        void FixedUpdate()
        {
            if (ev.IsMine) // ev.owner == me
            {
                if (on_floor)
                {
                    if (Input.GetKey(KeyCode.A))
                        transform.rotation = Quaternion.AngleAxis(rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                    if (Input.GetKey(KeyCode.D))
                        transform.rotation = Quaternion.AngleAxis(-rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                    if (Input.GetKey(KeyCode.W))
                        rb.AddForce(transform.forward * move_speed);

                    if (Input.GetKeyDown(KeyCode.J))
                    {
                        Debug.Log("jumping");
                        rb.AddForce(Vector3.up * jump_force);
                    }
                }
                else
                {
                    if (Input.GetKey(KeyCode.A))
                        transform.rotation = Quaternion.AngleAxis(rot_speed * air_turn_control * Time.deltaTime, Vector3.up) * transform.rotation;

                    if (Input.GetKey(KeyCode.D))
                        transform.rotation = Quaternion.AngleAxis(-rot_speed * air_turn_control * Time.deltaTime, Vector3.up) * transform.rotation;

                    if (Input.GetKey(KeyCode.W))
                        rb.AddForce(transform.forward * move_speed * air_move_control);
                }
            }
        }
    }
}
