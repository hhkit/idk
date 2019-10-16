namespace idk
{
    public class Transform
        : Component
    {
        public Vector3 position {
            get => Bindings.TransformGetPosition(handle);
            set => Bindings.TransformSetPosition(handle, value);
        }

        public Vector3 scale
        {
            get => Bindings.TransformGetScale(handle);
            set => Bindings.TransformSetScale(handle, value);
        }
    }
}
