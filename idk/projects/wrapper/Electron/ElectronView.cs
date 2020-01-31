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
            byte[] bytes;
            var formatter = new BinaryFormatter();
            using (MemoryStream stream = new MemoryStream())
            {
                foreach (var param in parameters)
                    formatter.Serialize(stream, param);
                bytes = stream.ToArray();
            }
            Bindings.ViewExecRPC(handle, methodName, target, bytes);
        }
    }
}
