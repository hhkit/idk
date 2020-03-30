namespace idk
{
    public class Camera
        : Component
    {
        /// <summary>
        /// The field of view of the camera in degrees.
        /// </summary>
        public float fieldOfView
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

        public int UseFog
        {
            get => Bindings.CameraGetUseFog(handle);
            set => Bindings.CameraSetUseFog(handle, value);
        }

        public int UseBloom
        {
            get => Bindings.CameraGetUseBloom(handle);
            set => Bindings.CameraSetUseBloom(handle, value);
        }

        public Color FogColor
        {
            get => Bindings.CameraGetFogColor(handle);
            set => Bindings.CameraSetFogColor(handle, value);
        }
        public float FogDensity
        {
            get => Bindings.CameraGetFogDensity(handle);
            set => Bindings.CameraSetFogDensity(handle, value);
        }

        public Vector3 Threshold
        {
            get => Bindings.CameraGetThreshold(handle);
            set => Bindings.CameraSetThreshold(handle, value);
        }

        public float BlurStrength
        {
            get => Bindings.CameraGetBlurStrength(handle);
            set => Bindings.CameraSetBlurStrength(handle, value);
        }

        public float BlurScale
        {
            get => Bindings.CameraGetBlurScale(handle);
            set => Bindings.CameraSetBlurScale(handle, value);
        }
    }
}
