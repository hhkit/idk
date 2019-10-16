namespace idk
{
    public class Component
        : Object
    {
        public GameObject gameObject
        {
            get => new GameObject(Bindings.ComponentGetGameObject(handle));
        }

        public Transform transform
        {
            get => gameObject.transform;
        }
    }
}
