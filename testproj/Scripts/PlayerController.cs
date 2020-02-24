using idk;
using System.Collections;

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
        int jump_count = 1;

        RigidBody rb;
        ElectronView ev;

        internal bool transfer = false;
        internal Client p = null;

        [ElecRPC]
        void ExecuteMe()
        {
            Debug.Log("RPC executed");
        }
        [ElecRPC]
        void ExecuteMeWithArgs(string param, int val, string param2)
        {
            Debug.Log("RPC executed " + param + "," + val + "," + param2);
        }

        [ElecRPC]
        void InformMe(ElectronView view)
        {
            Debug.Log("Inform me about " + view.InstantiationId);
        }

        [ElecRPC]
        void ExecuteMeCheckingForSender(string param, ElectronMessageInfo info)
        {
            Debug.Log("RPC from " + info.sender + " executed");
        }

        void Start()
        {
            rb = GetComponent<RigidBody>();
            ev = GetComponent<ElectronView>();
        }

        void OnTriggerEnter(Collider other)
        {
            if (other.tag == "futanari")
            {
                on_floor = true;
                if (transfer && p != null)
                        ev.TransferOwnership(p);
            }
        }
        void OnTriggerExit(Collider other)
        {
            if (other.tag == "futanari")
                on_floor = false;
        }
        IEnumerator JumpCooldown()
        {
            yield return new WaitForSeconds(0.05);
            jump_count++;
        }
        void FixedUpdate()
        {

            if (ev.IsMine) // ev.owner == me
            {
                if (Input.GetKey(KeyCode.Shift))
                {
                    if (Input.GetKeyDown(KeyCode.E))
                        ev.RPC("ExecuteMeCheckingForSender", RPCTarget.Server, "help me");
                }

                if (on_floor)
                {
                    if (Input.GetKey(KeyCode.A))
                        transform.rotation = Quaternion.AngleAxis(rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                    if (Input.GetKey(KeyCode.D))
                        transform.rotation = Quaternion.AngleAxis(-rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                    if (Input.GetKey(KeyCode.W))
                        rb.AddForce(transform.forward * move_speed);

                    if (jump_count > 0)
                    {
                        if (Input.GetKey(KeyCode.J))
                        {
                            --jump_count;
                            StartCoroutine(JumpCooldown());
                            rb.AddForce(Vector3.up * jump_force);
                        }
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
