namespace idk
{
    public class Animator
        : Component
    {
        public void Play(string name)
             => Bindings.AnimatorPlay(handle, name);

        public void CrossFade(string name)
           => Bindings.AnimatorCrossFade(handle, name);

        public void CrossFade(string name, float time)
          => Bindings.AnimatorCrossFade(handle, name, time);

        public void Resume()
             => Bindings.AnimatorResume(handle);

        public void Pause()
             => Bindings.AnimatorPause(handle);

        public void Stop()
             => Bindings.AnimatorStop(handle);

        public string DefaultStateName()
            => Bindings.AnimatorDefaultStateName(handle);

        public string CurrentStateName()
           => Bindings.AnimatorCurrentStateName(handle);

        public string BlendStateName()
           => Bindings.AnimatorBlendStateName(handle);

        public bool IsPlaying()
           => Bindings.AnimatorIsPlaying(handle);

        public bool IsBlending()
          => Bindings.AnimatorIsBlending(handle);

        public bool HasCurrAnimEnded()
           => Bindings.AnimatorHasCurrAnimEnded(handle);

        public bool HasState(string name)
          => Bindings.AnimatorHasState(handle, name);

    }
}
