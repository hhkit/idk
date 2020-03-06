namespace idk
{
    public class RectTransform
        : Component
    {
        public Vector2 offsetMin
        {
            get => Bindings.RectTransformGetOffsetMin(handle);
            set => Bindings.RectTransformSetOffsetMin(handle, value);
        }
        public Vector2 offsetMax
        {
            get => Bindings.RectTransformGetOffsetMax(handle);
            set => Bindings.RectTransformSetOffsetMax(handle, value);
        }
        public Vector2 anchorMin
        {
            get => Bindings.RectTransformGetAnchorMin(handle);
            set => Bindings.RectTransformSetAnchorMin(handle, value);
        }
        public Vector2 anchorMax
        {
            get => Bindings.RectTransformGetAnchorMax(handle);
            set => Bindings.RectTransformSetAnchorMax(handle, value);
        }
        public Vector2 anchorPos
        {
            get => Bindings.RectTransformGetAnchorPos(handle);
            set => Bindings.RectTransformSetAnchorPos(handle, value);
        }

        public Vector2 pivot
        {
            get => Bindings.RectTransformGetPivot(handle);
            set => Bindings.RectTransformSetPivot(handle, value);
        }
    }
}