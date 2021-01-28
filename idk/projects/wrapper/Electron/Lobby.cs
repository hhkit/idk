using System.Collections.Generic;

namespace idk
{
    public enum LobbyType
    {
        /// <summary>
        /// Visible to friends or invitees only
        /// </summary>
        Private = 1,
        /// <summary>
        /// Visible to everyone
        /// </summary>
        Public = 2
    }

    public class Lobby
    {
        internal ulong id;

        /// <summary>
        /// Get the number of members in this lobby.
        /// </summary>
        public int memberCount => Bindings.NetworkGetLobbyNumMembers(id);

        public ulong ID => id;

        internal Lobby(ulong id)
        {
            this.id = id;
        }

        /// <summary>
        /// Get value of associated key. If key not found, returns empty string.
        /// </summary>
        public string GetData(string key) => Bindings.NetworkGetLobbyData(id, key);

        public override bool Equals(object obj)
        {
            return obj is Lobby lobby &&
                   id == lobby.id;
        }

        public override int GetHashCode()
        {
            return id.GetHashCode();
        }

        public static bool operator ==(Lobby left, Lobby right)
        {
            return EqualityComparer<Lobby>.Default.Equals(left, right);
        }

        public static bool operator !=(Lobby left, Lobby right)
        {
            return !(left == right);
        }

        public static implicit operator bool(Lobby lobby) => lobby != null && lobby.id != 0;
    }
}
