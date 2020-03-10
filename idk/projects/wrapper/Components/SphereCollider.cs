namespace idk
{
    public class SphereCollider
        : Collider
    {
        public Vector3 center
        {
            get => Bindings.ColliderSphereGetCenter(handle);
            set => Bindings.ColliderSphereSetCenter(handle, value);
        }
        public float radius
        {
            get => Bindings.ColliderSphereGetRadius(handle);
            set => Bindings.ColliderSphereSetRadius(handle, value);
        }
    }
}
