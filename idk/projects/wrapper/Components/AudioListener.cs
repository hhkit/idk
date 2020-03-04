//////////////////////////////////////////////////////////////////////////////////
//@file		AudioListener.cs
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		02 MAR 2020
//@brief	A GameObject Component to listen stuff. If there is only one or none, 3D sound is enabled. Else 3D sound might sound 2D ish. 
//////////////////////////////////////////////////////////////////////////////////


namespace idk
{
    public class AudioListener
        : Component
    {
        //------------------
        //Public Variables
        //------------------

        public bool enabled
        {
            get => Bindings.AudioListenerGetEnabledState(handle);
            set => Bindings.AudioListenerSetEnabledState(handle, value);
        }

    }
}
