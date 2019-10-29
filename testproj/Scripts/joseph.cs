using idk;

namespace TestAndSeek
{
    public class AnimationTest 
        : MonoBehavior
    {
      
        private Animator anim;
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

        public override void Start()
        {
            anim = gameObject.GetComponent<Animator>();
            
            if (anim)
                Debug.Log("found animator");
        }


        public override void FixedUpdate()
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
