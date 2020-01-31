namespace idk
{
    public struct AnimationState
    {
        public static implicit operator bool(AnimationState s)
        {
            return s.valid;
        }

        public bool valid;
        public bool loop;
        public float normalizedTime;
        public float speed;
        public float duration;
        public float fps;
        
    }
}
