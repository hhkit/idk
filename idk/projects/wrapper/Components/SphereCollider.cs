namespace idk
{
    public class SphereCollider
        : Collider
    {
        Vector3 center
        {
            get => Bindings.ColliderSphereGetCenter(handle);
            set => Bindings.ColliderSphereSetCenter(handle, value);
        }
        float radius
        {
            get => Bindings.ColliderSphereGetRadius(handle);
            set => Bindings.ColliderSphereSetRadius(handle, value);
        }
    }
}
