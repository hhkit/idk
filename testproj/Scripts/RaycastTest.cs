using idk;

namespace TestAndSeek
{
    public class RaycastTest
        : MonoBehavior
    {
        public void Update()
        {
            if (Physics.Raycast(transform.position, transform.forward, out RaycastHit hit))
                Debug.Log("Hitted" + hit.collider.gameObject.name);
            else
                Debug.Log("Hitted no one");
        }
    }
}
