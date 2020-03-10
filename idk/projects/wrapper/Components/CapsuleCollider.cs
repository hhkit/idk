namespace idk
{
    public class CapsuleCollider
        : Collider
    {
        public Vector3 center
        {
            get => Bindings.ColliderCapsuleGetCenter(handle);
            set => Bindings.ColliderCapsuleSetCenter(handle, value);
        }
        public Vector3 direction
        {
            get => Bindings.ColliderCapsuleGetDirection(handle);
            set => Bindings.ColliderCapsuleSetDirection(handle, value);
        }
        public float height
        {
            get => Bindings.ColliderCapsuleGetHeight(handle);
            set => Bindings.ColliderCapsuleSetHeight(handle, value);
        }
        public float radius
        {
            get => Bindings.ColliderCapsuleGetRadius(handle);
            set => Bindings.ColliderCapsuleSetRadius(handle, value);
        }
    }
}
