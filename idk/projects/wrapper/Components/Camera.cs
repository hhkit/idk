namespace idk
{
    public class Camera
        : Component
    {
        public Rad fieldOfView
        {
            get => Bindings.CameraGetFOV(handle);
            set => Bindings.CameraSetFOV(handle, value);
        }

        public float nearClipPlane
        {
            get => Bindings.CameraGetNearPlane(handle);
            set => Bindings.CameraSetNearPlane(handle, value);
        }
        public float farClipPlane
        {
            get => Bindings.CameraGetFarPlane(handle);
            set => Bindings.CameraSetFarPlane(handle, value);
        }

        public Rect rect
        {
            get => Bindings.CameraGetViewport(handle);
            set => Bindings.CameraSetViewport(handle, value);
        }

        public bool enabled
        {
            get => Bindings.CameraGetEnabledState(handle);
            set => Bindings.CameraSetEnabledState(handle, value);
        }

        public int cullingMask
        {
            get => Bindings.CameraGetCullingMask(handle);
            set => Bindings.CameraSetCullingMask(handle, value);
        }

    }
}
