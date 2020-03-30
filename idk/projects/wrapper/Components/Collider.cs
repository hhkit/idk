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

        public float staticFriction
        {
            get => Bindings.ColliderGetStaticFriction(handle);
            set => Bindings.ColliderSetStaticFriction(handle, value);
        }

        public float dynamicFriction
        {
            get => Bindings.ColliderGetDynamicFriction(handle);
            set => Bindings.ColliderSetDynamicFriction(handle, value);
        }

        public float bounciness
        {
            get => Bindings.ColliderGetBounciness(handle);
            set => Bindings.ColliderSetBounciness(handle, value);
        }
    }
}
