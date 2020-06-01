namespace idk
{
    public class Animator
        : Component
    {
        public Vector3 offset
        {
            get => Bindings.AnimatorGetOffset(handle);
            set => Bindings.AnimatorSetOffset(handle, value);
        }

        public bool Play(string name)
             => Bindings.AnimatorPlay(handle, name);
        public bool Play(string name, string layer)
             => Bindings.AnimatorPlay(handle, name, layer);

        public bool BlendTo(string name)
           => Bindings.AnimatorBlendTo(handle, name);

        public bool BlendTo(string name, float time)
          => Bindings.AnimatorBlendTo(handle, name, time);

        public bool BlendTo(string name, string layer)
          => Bindings.AnimatorBlendTo(handle, name, 0.2f, layer);

        public bool BlendTo(string name, float time, string layer)
          => Bindings.AnimatorBlendTo(handle, name, time, layer);

        public bool BlendToInSeconds(string name)
           => Bindings.AnimatorBlendToInSeconds(handle, name);

        public bool BlendToInSeconds(string name, float timeInSeconds)
          => Bindings.AnimatorBlendToInSeconds(handle, name, timeInSeconds);

        public bool BlendToInSeconds(string name, string layer)
          => Bindings.AnimatorBlendToInSeconds(handle, name, 0.0f, layer);

        public bool BlendToInSeconds(string name, float timeInSeconds, string layer)
          => Bindings.AnimatorBlendToInSeconds(handle, name, timeInSeconds, layer);

        public bool Resume()
             => Bindings.AnimatorResume(handle);
        public bool Resume(string layer)
             => Bindings.AnimatorResume(handle, layer);

        public bool Pause()
             => Bindings.AnimatorPause(handle);
        public bool Pause(string layer)
             => Bindings.AnimatorPause(handle, layer);

        public bool Stop()
             => Bindings.AnimatorStop(handle);
        public bool Stop(string layer)
             => Bindings.AnimatorStop(handle, layer);

        public AnimationState GetState(string name)
            => Bindings.AnimatorGetState(handle, name);
         
        public AnimationState GetState(string name, string layer)
             => Bindings.AnimatorGetState(handle,name, layer);
        
        public string DefaultStateName()
            => Bindings.AnimatorDefaultStateName(handle);
        public string DefaultStateName(string layer)
           => Bindings.AnimatorDefaultStateName(handle, layer);

        public string CurrentStateName()
           => Bindings.AnimatorCurrentStateName(handle);
        public string CurrentStateName(string layer)
           => Bindings.AnimatorCurrentStateName(handle, layer);

        public float CurrentStateTime()
           => Bindings.AnimatorCurrentStateTime(handle);
        public float CurrentStateTime(string layer)
           => Bindings.AnimatorCurrentStateTime(handle, layer);

        public string BlendStateName()
           => Bindings.AnimatorBlendStateName(handle);
        public string BlendStateName(string layer)
           => Bindings.AnimatorBlendStateName(handle, layer);

        public bool IsPlaying()
           => Bindings.AnimatorIsPlaying(handle);
        public bool IsPlaying(string layer)
           => Bindings.AnimatorIsPlaying(handle, layer);

        public bool IsBlending()
          => Bindings.AnimatorIsBlending(handle);
        public bool IsBlending(string layer)
          => Bindings.AnimatorIsBlending(handle, layer);

        public bool HasCurrAnimEnded()
           => Bindings.AnimatorHasCurrAnimEnded(handle);
        public bool HasCurrAnimEnded(string layer)
           => Bindings.AnimatorHasCurrAnimEnded(handle, layer);

        public bool HasState(string name)
          => Bindings.AnimatorHasState(handle, name);
        public bool HasState(string name, string layer)
          => Bindings.AnimatorHasState(handle, name, layer);
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

        public float GetWeight()
      => Bindings.AnimatorGetWeight(handle);
        public float GetWeight(string name)
      => Bindings.AnimatorGetWeight(handle, name);

        public bool SetWeight(float weight)
       => Bindings.AnimatorSetWeight(handle, weight);
        public bool SetWeight(float weight, string name)
       => Bindings.AnimatorSetWeight(handle, weight, name);
    }
}
