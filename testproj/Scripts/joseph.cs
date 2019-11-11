using idk;

namespace TestAndSeek
{
    public class AnimationTest 
        : MonoBehavior
    {
      
        private Animator anim;
        //private Transform t;
        private bool blend_next = false;
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
                Debug.Log("found animator");
        }


        void FixedUpdate()
        {
            if (anim)
            {
                if (Input.GetKey(KeyCode.A))
                    PlayOrBlend("idle");
                else if (Input.GetKey(KeyCode.S))
                    PlayOrBlend("walk");
                else if (Input.GetKey(KeyCode.D))
                    PlayOrBlend("test");
                else if (Input.GetKey(KeyCode.F))
                    PlayOrBlend("test2");
                else if (Input.GetKey(KeyCode.G))
                    PlayOrBlend("BLAH");

                else if (Input.GetKey(KeyCode.B))
                    blend_next = !blend_next;
            }
        }
    }

   
}
