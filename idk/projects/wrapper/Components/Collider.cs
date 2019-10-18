namespace idk
{
    public class Collider
        : Component
    {
        public RigidBody attachedRigidbody
        {
            get => gameObject.GetComponent<RigidBody>();
        }

        public bool enabled
        {
            get => Bindings.ColliderGetEnabled(handle);
            set => Bindings.ColliderSetEnabled(handle, value);
        }

        public bool isTrigger
        {
            get => Bindings.ColliderGetTrigger(handle);
            set => Bindings.ColliderSetTrigger(handle, value);
        }
    }
}
