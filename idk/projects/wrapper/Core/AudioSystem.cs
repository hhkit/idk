//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.cs
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		01 NOV 2019
//@brief	Static calls to the Audio System directly

//////////////////////////////////////////////////////////////////////////////////


namespace idk
{
    public class AudioSystem
    {


        //------------------
        //Public Functions
        //------------------

        //Clamps to 0..1.
        public static void SetMasterVolume(float newVolume)
            => Bindings.AudioSystemSetVolume(newVolume);

        public static void StopAll()
            => Bindings.AudioSystemStopAll();

    }
}
