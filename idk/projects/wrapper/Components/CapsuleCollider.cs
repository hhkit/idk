namespace idk
{
    public class CapsuleCollider
        : Collider
    {
        Vector3 center
        {
            get => Bindings.ColliderCapsuleGetCenter(handle);
            set => Bindings.ColliderCapsuleSetCenter(handle, value);
        }
        Vector3 direction
        {
            get => Bindings.ColliderCapsuleGetDirection(handle);
            set => Bindings.ColliderCapsuleSetDirection(handle, value);
        }
        float height
        {
            get => Bindings.ColliderCapsuleGetHeight(handle);
            set => Bindings.ColliderCapsuleSetHeight(handle, value);
        }
        float radius
        {
            get => Bindings.ColliderCapsuleGetRadius(handle);
            set => Bindings.ColliderCapsuleSetRadius(handle, value);
        }
    }
}
