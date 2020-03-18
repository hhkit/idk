//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.cs
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		01 NOV 2019
//@brief	Static calls to the Audio System directly

//////////////////////////////////////////////////////////////////////////////////


namespace idk
{
    public class Audio
    {
        //------------------
        //Public Functions
        //------------------

        /// <summary>
        /// Clamps to 0...1
        /// </summary>
        public static float masterVolume
        {
            get => Bindings.AudioSystemGetMASTERVolume();
            set => Bindings.AudioSystemSetMASTERVolume(value);
        }
        public static float sfxVolume
        {
            get => Bindings.AudioSystemGetSFXVolume();
            set => Bindings.AudioSystemSetSFXVolume(value);
        }
        public static float musicVolume
        {
            get => Bindings.AudioSystemGetMUSICVolume();
            set => Bindings.AudioSystemSetMUSICVolume(value);
        }
        public static float ambientVolume
        {
            get => Bindings.AudioSystemGetAMBIENTVolume();
            set => Bindings.AudioSystemSetAMBIENTVolume(value);
        }
        public static float dialogueVolume
        {
            get => Bindings.AudioSystemGetDIALOGUEVolume();
            set => Bindings.AudioSystemSetDIALOGUEVolume(value);
        }

        public static bool pausedState_Master
        {
            get => Bindings.AudioSystemGetMASTERPause();
            set => Bindings.AudioSystemSetMASTERPause(value);
        }
        public static bool pausedState_Sfx
        {
            get => Bindings.AudioSystemGetSFXPause();
            set => Bindings.AudioSystemSetSFXPause(value);
        }
        public static bool pausedState_Music
        {
            get => Bindings.AudioSystemGetMUSICPause();
            set => Bindings.AudioSystemSetMUSICPause(value);
        }
        public static bool pausedState_Ambient
        {
            get => Bindings.AudioSystemGetAMBIENTPause();
            set => Bindings.AudioSystemSetAMBIENTPause(value);
        }
        public static bool pausedState_Dialogue
        {
            get => Bindings.AudioSystemGetDIALOGUEPause();
            set => Bindings.AudioSystemSetDIALOGUEPause(value);
        }

        public static void StopAll()
            => Bindings.AudioSystemStopAll();

    }
}
