namespace idk
{
    public class BoxCollider
        : Collider
    {
        Vector3 center 
        { 
            get => Bindings.ColliderBoxGetCenter(handle);
            set => Bindings.ColliderBoxSetCenter(handle, value);
        }
        Vector3 size
        {
            get => Bindings.ColliderBoxGetSize(handle);
            set => Bindings.ColliderBoxSetSize(handle, value);
        }
    }
}
