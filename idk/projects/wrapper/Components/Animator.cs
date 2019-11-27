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
        public int GetInt(string name)
         => Bindings.AnimatorGetInt(handle, name);
        public float GetFloat(string name)
        => Bindings.AnimatorGetFloat(handle, name);

        public bool GetBool(string name)
        => Bindings.AnimatorGetBool(handle, name);

        public bool GetTrigger(string name)
        => Bindings.AnimatorGetTrigger(handle, name);
        public bool SetInt(string name, int val)
        => Bindings.AnimatorSetInt(handle, name, val);
        public bool SetFloat(string name, float val)
        => Bindings.AnimatorSetFloat(handle, name, val);

        public bool SetBool(string name, bool val)
        => Bindings.AnimatorSetBool(handle, name, val);

        public bool SetTrigger(string name, bool val)
        => Bindings.AnimatorSetTrigger(handle, name, val);

    }
}
