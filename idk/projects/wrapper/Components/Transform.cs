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

        public Vector3 localPosition
        {
            get => Bindings.TransformGetLocalPosition(handle);
            set => Bindings.TransformSetLocalPosition(handle, value);
        }

        public Quaternion localRotation
        {
            get => Bindings.TransformGetLocalRotation(handle);
            set => Bindings.TransformSetLocalRotation(handle, value);
        }

        public Vector3 localScale
        {
            get => Bindings.TransformGetLocalScale(handle);
            set => Bindings.TransformSetLocalScale(handle, value);
        }

        public Vector3 forward
        {
            get => Bindings.TransformForward(handle);
            set => Bindings.TransformSetForward(handle, value);
        }

        public Vector3 up 
        {
            get => Bindings.TransformUp(handle);
        }

        public Vector3 right
        {
            get => Bindings.TransformRight(handle);
        }

        public GameObject parent
        {
            get
            {
                var parent = Bindings.TransformGetParent(handle);
                return parent != 0 ? new GameObject(parent) : null;
            }
        }

        public void SetParent(Transform parent, bool preserve_global)
        {
            ulong parent_id = parent ? parent.gameObject.handle : 0;
            Bindings.TransformSetParent(handle, parent_id, preserve_global);
        }

        public void SetParent(GameObject parent, bool preserve_global)
        {
            ulong parent_id = parent.handle;
            Bindings.TransformSetParent(handle, parent_id, preserve_global);
        }

        public GameObject[] GetChildren()
        {
            return Bindings.TransformGetChildren(handle);
        }
    }
}
