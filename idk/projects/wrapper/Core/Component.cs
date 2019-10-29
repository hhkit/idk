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

        public string tag
        {
            get => gameObject.tag;
        }

        public T GetComponent<T>() where T : Component, new()
        {
            return gameObject.GetComponent<T>();
        }
    }
}
