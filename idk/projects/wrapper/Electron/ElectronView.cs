using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace idk
{
    public class ElectronView
        : Component
    {
        private static BinaryFormatter formatter = new BinaryFormatter();   

        public uint InstantiationId { get => Bindings.ViewGetNetworkId(handle); }
        public void TransferOwnership(int newOwnerId) => Bindings.ViewTransferOwnership(handle, newOwnerId);
        public void RPC(string methodName, RPCTarget target, params object[] parameters)
        {
            byte[][] bytes = new byte[parameters.Length][];
            var formatter = new BinaryFormatter();
            for (int count = 0; count < parameters.Length; ++count)
            {
                using (MemoryStream stream = new MemoryStream())
                {
                    formatter.Serialize(stream, parameters[count]);
                    bytes[count] = stream.ToArray();
                }
            }
            Bindings.ViewExecRPC(handle, methodName, target, bytes);
        }
        internal static object[] Reserialize(byte[][] bytes)
        {
            Debug.Log("OBJ SIZE: " + bytes.Length);
            object[] output = new object[bytes.Length];
            for (int count = 0; count < bytes.Length; ++count)
            {
                Debug.Log("TEST1" + bytes.Length);
                var formatter = new BinaryFormatter();
                using (MemoryStream stream = new MemoryStream(bytes[count]))
                {
                    Debug.Log("TEST2" + bytes.Length);
                    output[count] = formatter.Deserialize(stream);
                }
            }
            return output;
        }
    }
}
