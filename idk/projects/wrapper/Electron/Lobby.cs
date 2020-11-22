using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Lobby
    {
        internal ulong id;

        public static Lobby invalid => new Lobby();

        /// <summary>
        /// Get the number of members in this lobby.
        /// </summary>
        public int memberCount => Bindings.NetworkGetLobbyNumMembers(id);

        /// <summary>
        /// Get value of associated key. If key not found, returns empty string.
        /// </summary>
        public string GetData(string key) => Bindings.NetworkGetLobbyData(id, key);

        public ulong ID => id;
    }
}
