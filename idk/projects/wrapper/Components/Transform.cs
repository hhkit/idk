namespace idk
{
    public class Transform
        : Component
    {
        public Vector3 position
        {
            get => Bindings.TransformGetPosition(handle);
            set => Bindings.TransformSetPosition(handle, value);
        }

        public Quaternion rotation
        {
            get => Bindings.TransformGetRotation(handle);
            set => Bindings.TransformSetRotation(handle, value);
        }

        public Vector3 scale
        {
            get => Bindings.TransformGetScale(handle);
            set => Bindings.TransformSetScale(handle, value);
        }

        public Vector3 forward
        {
            get => Bindings.TransformForward(handle);
        }

        public Vector3 up 
        {
            get => Bindings.TransformUp(handle);
        }

        public Vector3 right
        {
            get => Bindings.TransformRight(handle);
        }
    }
}
