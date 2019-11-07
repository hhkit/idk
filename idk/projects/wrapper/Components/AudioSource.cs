//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSource.cs
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		01 NOV 2019
//@brief	A GameObject Component that holds AudioClips to play sounds.
/*
			All the AudioClips data such as volume, loop, are controlled by this
            AudioSource. If you want multiple AudioClips with their own data, add
            multiple AudioSources instead.
            EG: Have Childrens of AudioSource so you can have unique name to each
            audioSource.
*/
//////////////////////////////////////////////////////////////////////////////////


namespace idk
{
    public class AudioSource
        : Component
    {
        //------------------
        //Public Variables
        //------------------

        //Volume of the audiosource. Negative value inverts the signal. Range: [-infinite, infinite]
        public float volume {
            get => Bindings.AudioSourceGetVolume(handle);
            set => Bindings.AudioSourceSetVolume(handle, value);
        }
        //The pitch of the audiosource. Range: [0, infinite]
        public float pitch {
            get => Bindings.AudioSourceGetPitch(handle);
            set => Bindings.AudioSourceSetPitch(handle, value);
        }
        //Is the audiosource looping?
        public bool loop {
            get => Bindings.AudioSourceGetLoop(handle);
            set => Bindings.AudioSourceSetLoop(handle, value);
        }


        //------------------
        //Public Functions
        //------------------

        //Plays an audioclip in the audiosource. If index is out of range, no sound will play.
        public void Play(int index = 0)
            => Bindings.AudioSourcePlay(handle, index);

        //Plays all audioclip in the audiosource.
        public void PlayAll()
            => Bindings.AudioSourcePlayAll(handle);

        //Plays an audioclip in the audiosource. If index is out of range, no sound will stop.
        public void Stop(int index = 0)
            => Bindings.AudioSourceStop(handle, index);

        //Stop all audioclip in the audiosource.
        public void StopAll()
            => Bindings.AudioSourceStopAll(handle);

        //Gets the number of audioclips in audiosource.
        public int Size()
            => Bindings.AudioSourceSize(handle);

        //Is the audioclip playing?
        public bool IsAudioClipPlaying(int index)
            => Bindings.AudioSourceIsAudioClipPlaying(handle, index);

        //Are any audioclip in this audiosource playing?
        public bool IsAnyAudioClipPlaying()
            => Bindings.AudioSourceIsAnyAudioClipPlaying(handle);
    }
}
