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


        public static void StopAll()
            => Bindings.AudioSystemStopAll();

    }
}
