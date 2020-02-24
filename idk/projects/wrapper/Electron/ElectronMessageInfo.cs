
namespace idk
{
    /// <summary>
    /// User can check the message info of the message through this data struct.
    /// </summary>
    public struct ElectronMessageInfo
    {
        private ulong  view_handle;
        private int fromID;
        private ushort frameStamp;

        /// <summary>
        /// The sender of the message.
        /// </summary>
        public Client sender { get=> new Client(fromID); }

        /// <summary>
        /// The view on which the RPC was executed on.
        /// </summary>
        public ElectronView electronView { get => new ElectronView() { handle = view_handle }; }
    }
}
