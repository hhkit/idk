namespace idk
{
    public class Component
        : IObject
    {
        internal ulong handle = 0;

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
