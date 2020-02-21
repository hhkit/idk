using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace idk
{
    public class ElectronView
        : Component
    {
        private static BinaryFormatter formatter = new BinaryFormatter();

        public bool IsMine { get => Bindings.ViewIsMine(handle); }
        public uint InstantiationId { get => Bindings.ViewGetNetworkId(handle); }
        public void TransferOwnership(Player newOwner) => Bindings.ViewTransferOwnership(handle, newOwner != null ? newOwner.ActorNumber : -1);

        public void DestroyObject() => Bindings.ViewDestroy(handle);

        public void RPC(string methodName, RPCTarget target, params object[] parameters)
        {
            if (ElectronNetwork.isHost)
            {
                // placeholder for exceptions
            }
            else
            {
                if (target != RPCTarget.Server)
                    throw new InvalidRPCTargetException("Client may only target server");
            }

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

        public void RPC(string methodName, Player targetPlayer, params object[] parameters)
        {
            if (!ElectronNetwork.isHost)
                throw new InvalidRPCTargetException("Only the host may target players.");

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
            throw new System.NotImplementedException();
        }
        internal static object[] Reserialize(byte[][] bytes)
        {
            object[] output = new object[bytes.Length];
            for (int count = 0; count < bytes.Length; ++count)
            {
                var formatter = new BinaryFormatter();
                using (MemoryStream stream = new MemoryStream(bytes[count]))
                {
                    output[count] = formatter.Deserialize(stream);
                }
            }
            return output;
        }
    }
}
