namespace idk
{
    public class ElectronView
        : Component
    {
        public int InstantiationId { get => Bindings.ViewGetNetworkId(handle); }
        public void TransferOwnership(int newOwnerId) => Bindings.ViewTransferOwnership(handle, newOwnerId);
        public void RPC(string methodName, RPCTarget target, params object[] parameters) => Bindings.ViewExecRPC(handle, methodName, target, parameters);
    }
}
