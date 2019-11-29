using idk;

namespace TestAndSeek
{
    public class AnimationTest 
        : MonoBehavior
    {
      
        private Animator anim;
        //private Transform t;
        private bool blend_next = false;
        public float elapsed = 0.0f;
        public bool w_pressed = false;
        public int Int = 0;
        public float Float = 0.0f;
        public bool Bool = false;
        public bool Trigger = false;
        public float Speed = 0.0f;
        public Prefab playerChar;
        private int count = 0;
        public AnimationTest()
        {
            Debug.Log("AnimationTest is constructed");
        }
        public void PlayOrBlend(string name)
        {
            if (blend_next)
            {
                anim.CrossFade(name);
            }
            else
                anim.Play(name);
        }

        void Start()
        {
            anim = gameObject.GetComponent<Animator>();
            Quaternion q = new Quaternion();
            q.eulerAngles = new Vector3(0, 0, 0);
            gameObject.GetComponent<Transform>().localRotation = q;

            if (anim)
            {
                Debug.Log("found animator");
                Int = anim.GetInt("int");
                Float = anim.GetFloat("float");
                Bool = anim.GetBool("bool");
                Trigger = anim.GetTrigger("trigger");
            }
        }


        void FixedUpdate()
        {
            if (anim)
            {
                // if (Input.GetKey(KeyCode.A))
                // {
                //     anim.SetInt("int", Int >= 0 ? 1 : 0);
                // }
                // else if (Input.GetKey(KeyCode.S))
                // {
                //     anim.SetFloat("float", Float >= 0.0f ? 0.6f : 0.0f);
                // }
                // else if (Input.GetKey(KeyCode.D))
                // {
                //     anim.SetBool("bool", !Bool);
                // }
                if (Input.GetKey(KeyCode.F) && count == 0)
                {
                    GameObject obj = playerChar.Instantiate();
                    Animator anim = obj.GetComponent<Animator>();
                    anim.Play(anim.DefaultStateName());
                    ++count;
                }
                // else if (Input.GetKey(KeyCode.G))
                // {
                //     anim.CrossFade("test2");
                // }
                anim.SetFloat("speed", Speed);
                // if (Input.GetKey(KeyCode.W))
                // {
                //     if (!w_pressed)
                //         elapsed = 0.0f;
                //     Speed = Mathf.Lerp(Speed, 1.0f, elapsed);
                //     anim.SetFloat("speed", Speed);
                //     elapsed += 0.02f;
                //     w_pressed = true;
                // }
                // else if (Speed > 0.0f)
                // {
                //     if (w_pressed)
                //         elapsed = 0.0f;
                //     Speed = Mathf.Lerp(Speed, 0.0f, elapsed);
                //     anim.SetFloat("speed", Speed);
                //     elapsed += 0.02f;
                //     w_pressed = false;
                // }
                // Int = anim.GetInt("int");
                // Float = anim.GetFloat("float");
                // Bool = anim.GetBool("bool");
                // Trigger = anim.GetTrigger("trigger");
            }
        }
    }

   
}
