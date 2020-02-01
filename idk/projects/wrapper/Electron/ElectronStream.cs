using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class ElectronStream
    {
        internal IntPtr cppstream;

        public void Serialize(ref bool myBool) { }
        public void Serialize(ref int myInt) { }
        public void Serialize(ref string value) { }
        public void Serialize(ref char value) { }
        public void Serialize(ref short value) { }
        public void Serialize(ref float value) { }
        public void Serialize(ref Vector3 value) { }
        public void Serialize(ref Vector2 value) { }
        public void Serialize(ref Quaternion value) { }
        public bool IsWriting { get; }
        public bool IsReading { get; }
        public int Count { get; }

    }
}
