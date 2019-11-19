namespace idk
{
    public class Camera
        : Component
    {
        public Rad FOV
        {
            get => Bindings.CameraGetFOV(handle);
            set => Bindings.CameraSetFOV(handle, value);
        }

        public float NearPlane
        {
            get => Bindings.CameraGetNearPlane(handle);
            set => Bindings.CameraSetNearPlane(handle, value);
        }
        public float FarPlane
        {
            get => Bindings.CameraGetFarPlane(handle);
            set => Bindings.CameraSetFarPlane(handle, value);
        }

        public Rect Viewport
        {
            get => Bindings.CameraGetViewport(handle);
            set => Bindings.CameraSetViewport(handle, value);
        }

        public bool IsEnabled
        {
            get => Bindings.CameraGetEnabledState(handle);
            set => Bindings.CameraSetEnabledState(handle, value);
        }

    }
}
