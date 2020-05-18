using System;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace idk
{
    [Serializable]
    public class ElectronView
        : Component, ISerializable
    {
        private static BinaryFormatter formatter = new BinaryFormatter();

        public static ElectronView FindViewByInstantiationid(uint id)
        {
            var new_handle = Bindings.ViewIdGetView(id);
            return new_handle != 0 ? new ElectronView() { handle = new_handle } : null;
        }

        public bool IsMine { get => Bindings.ViewIsMine(handle); }
        public uint InstantiationId { get => Bindings.ViewGetNetworkId(handle); }
        public void TransferOwnership(Client newOwner) => Bindings.ViewTransferOwnership(handle, newOwner != null ? newOwner.ActorNumber : -1);

        public void DestroyObject()
        {
            if (!ElectronNetwork.isHost)
                throw new InvalidNetworkOperationException("Only the Server may destroy objects");

            Bindings.ViewDestroy(handle);
        }

        public void RPC(string methodName, RPCTarget target, params object[] parameters)
        {
            if (InstantiationId == 0)
                throw new InvalidOperationException("ElectronView has ID == 0. It is not synced on the network!");

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

        /// <summary>
        /// Force one player's instance of the object to execute an RPC.
        /// </summary>
        /// <param name="methodName">Name of the method</param>
        /// <param name="targetPlayer">The player you're trying to target the RPC at</param>
        /// <param name="parameters">Parameters for the method</param>
        public void RPC(string methodName, Client targetPlayer, params object[] parameters)
        {
            if (!ElectronNetwork.isHost)
                throw new InvalidRPCTargetException("Only the server may target players.");

            Bindings.ViewExecRPCOnPlayer(handle, methodName, targetPlayer.ActorNumber, Serialize(parameters));
        }

        internal static byte[][] Serialize(object[] objects)
        {
            byte[][] bytes = new byte[objects.Length][];
            var formatter = new BinaryFormatter();
            for (int count = 0; count < objects.Length; ++count)
            {
                using (MemoryStream stream = new MemoryStream())
                {
                    formatter.Serialize(stream, objects[count]);
                    bytes[count] = stream.ToArray();
                }
            }

            return bytes;
        }

        internal static object[] Reserialize(byte[][] bytes)
        {
            object[] output = new object[bytes.Length];

            for (int count = 0; count < bytes.Length; ++count)
            {
                var formatter = new BinaryFormatter();
                if (bytes[count] != null)
                {
                    using (MemoryStream stream = new MemoryStream(bytes[count]))
                    {
                        output[count] = formatter.Deserialize(stream);
                    }
                }
            }
            return output;
        }

        public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("viewID", InstantiationId, typeof(uint));
        }

        public ElectronView()
        {
            handle = 0;
        }

        public ElectronView(SerializationInfo info, StreamingContext context)
        {
            var id = (uint) info.GetValue("viewID", typeof(uint));
            handle = Bindings.ViewIdGetView(id);
        }
    }
}
