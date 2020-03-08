namespace idk
{
    public class RectTransform
        : Component
    {
        /// <summary>
        /// The offset of the lower left corner of the rectangle relative to the lower left anchor.
        /// </summary>
        public Vector2 offsetMin
        {
            get => Bindings.RectTransformGetOffsetMin(handle);
            set => Bindings.RectTransformSetOffsetMin(handle, value);
        }

        /// <summary>
        /// The offset of the upper right corner of the rectangle relative to the upper right anchor.
        /// </summary>
        public Vector2 offsetMax
        {
            get => Bindings.RectTransformGetOffsetMax(handle);
            set => Bindings.RectTransformSetOffsetMax(handle, value);
        }

        /// <summary>
        /// The normalized position in the parent RectTransform that the lower left corner is anchored to.
        /// </summary>
        public Vector2 anchorMin
        {
            get => Bindings.RectTransformGetAnchorMin(handle);
            set => Bindings.RectTransformSetAnchorMin(handle, value);
        }

        /// <summary>
        /// The normalized position in the parent RectTransform that the upper right corner is anchored to.
        /// </summary>
        public Vector2 anchorMax
        {
            get => Bindings.RectTransformGetAnchorMax(handle);
            set => Bindings.RectTransformSetAnchorMax(handle, value);
        }

        /// <summary>
        /// The normalized position in this RectTransform that it rotates around.
        /// </summary>
        public Vector2 pivot
        {
            get => Bindings.RectTransformGetPivot(handle);
            set => Bindings.RectTransformSetPivot(handle, value);
        }

        /// <summary>
        /// The calculated rectangle in the local space of this RectTransform.
        /// </summary>
        public Rect rect => Bindings.RectTransformGetRect(handle);

        /// <summary>
        /// The position of the pivot of this RectTransform relative to the anchor reference point.
        /// The anchor reference point is the position of the anchors.
        /// If the anchors are not together, the anchor reference point is estimated using the pivot placement with respect to the four anchor points.
        /// Setting this property modifies offsetMin and offsetMax.
        /// </summary>
        public Vector2 anchoredPosition
        {
            get => Bindings.RectTransformGetAnchoredPosition(handle);
            set => Bindings.RectTransformSetAnchoredPosition(handle, value);
        }
    }
}