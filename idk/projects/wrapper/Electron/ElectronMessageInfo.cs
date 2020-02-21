
namespace idk
{
    struct ElectronMessageInfo
    {
        private int    fromID;
        private ulong  view_handle;
        private ushort frameStamp;

        public Player sender { get=> new Player(fromID); }
        public ElectronView electronView { get => new ElectronView() { handle = view_handle }; }
    }
}
