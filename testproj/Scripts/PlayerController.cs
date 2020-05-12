using idk;
using System.Collections;
using System;

namespace TestAndSeek
{
    [Serializable]
    struct PlayerInput
    {
        public bool turn_left;
        public bool turn_right;
        public bool move_forward;
        public bool jump;
    }

    class PlayerController
        : MonoBehavior
        , INetworkInputProcessor<PlayerInput>
    {
        public float jump_force = 25f;
        public float move_speed = 45f;

        public float air_turn_control = 0.02f;
        public float air_move_control = 0.25f;

        public float rot_speed = 90f;

        PlayerInput input;

        bool on_floor = false;
        int jump_count = 1;

        RigidBody rb;
        ElectronView ev;
        ElectronTransformView etransform;
        ElectronRigidbodyView erbv;

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
            etransform = GetComponent<ElectronTransformView>();
            erbv = GetComponent<ElectronRigidbodyView>();
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
            yield return new WaitForSeconds(0.5);
            jump_count++;
        }

        void FixedUpdate()
        {

            if (!ElectronNetwork.isConnected || ev.IsMine) // ev.owner == me
            {
                if (Input.GetKey(KeyCode.Shift))
                {
                    if (Input.GetKeyDown(KeyCode.E))
                        ev.RPC("ExecuteMeCheckingForSender", RPCTarget.Server, "help me");
                }

                input.turn_left = Input.GetKey(KeyCode.A);
                input.turn_right = Input.GetKey(KeyCode.D);
                input.move_forward = Input.GetKey(KeyCode.W);
                input.jump = Input.GetKey(KeyCode.J);

                ProcessInput(input);
            }
        }

        public PlayerInput GenerateInput()
        {
            return input;
        }

        public void ProcessInput(PlayerInput input)
        {
            Debug.Log("" + (input.turn_left ? 1 : 0)
                + (input.turn_right? 1 : 0)
                + (input.move_forward ? 1 : 0)
                + (input.jump ? 1 : 0));

            if (on_floor)
            {
                if (input.turn_left)
                    transform.rotation = Quaternion.AngleAxis(rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                if (input.turn_right)
                    transform.rotation = Quaternion.AngleAxis(-rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                if (input.move_forward)
                    rb.velocity = transform.forward * move_speed;

                if (jump_count > 0)
                {
                    if (input.jump)
                    {
                        --jump_count;
                        StartCoroutine(JumpCooldown());
                        rb.AddForce(Vector3.up * jump_force);
                    }
                }
            }
            else
            {
                if (input.turn_left)
                    transform.rotation = Quaternion.AngleAxis(rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                if (input.turn_right)
                    transform.rotation = Quaternion.AngleAxis(-rot_speed * Time.deltaTime, Vector3.up) * transform.rotation;

                if (input.move_forward)
                    rb.velocity = transform.forward * move_speed;

                if (input.turn_left)
                    transform.rotation = Quaternion.AngleAxis(rot_speed * air_turn_control * Time.deltaTime, Vector3.up) * transform.rotation;

                if (input.turn_right)
                    transform.rotation = Quaternion.AngleAxis(-rot_speed * air_turn_control * Time.deltaTime, Vector3.up) * transform.rotation;

                if (input.move_forward)
                    rb.velocity = transform.forward * move_speed * air_move_control;
            }
        }
    }
}
