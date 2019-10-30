using idk;

namespace TestAndSeek
{
    public class TestShou
        : MonoBehavior
    {
        public int i;
        public void PrintI()
        {
            Debug.Log("Eye Eye captain: " + i);
            var t = GetComponentInChildren<Test>();
            if (t)
                Debug.Log("I can't hear you: " + t.i);
        }
    }
}
