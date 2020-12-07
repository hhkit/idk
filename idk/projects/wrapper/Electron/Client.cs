namespace idk
{
    /// <summary>
    /// An identifier for a player on the network.
    /// </summary>
    public class Client
    {
        public static Client Server => new Client(-1);

        int connectionId;

        /// <summary>
        /// The Network Identifier for the player.
        /// Values range from 0 to 2 for up to 3 connections. -1 corresponds to the Server.
        /// </summary>
        public int actorNumber { get => connectionId; }

        /// <summary>
        /// The index of the player in the current lobby.
        /// Values range from 0 to 3. Server can be any number, unlike actorNumber.
        /// This value won't change until you leave the lobby.
        /// </summary>
        public int lobbyIndex => Bindings.NetworkClientLobbyIndex(connectionId);

        public string name => Bindings.NetworkClientName(connectionId);

        internal Client(int i)
        {
            connectionId = i;
        }

        public override string ToString()
        {
            return connectionId == -1 ? "Server" : ("Client " + actorNumber);
        }

        public static bool operator ==(Client lhs, Client rhs)
        {
            if ((object) lhs != null && (object) rhs != null)
                return lhs.connectionId == rhs.connectionId;
            else
                return (object)lhs == null && (object)rhs == null;
        }

        public static bool operator !=(Client lhs, Client rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object obj)
        {
            if ((obj == null) || !this.GetType().Equals(obj.GetType()))
            {
                return false;
            }
            else
            {
                return this.connectionId == ((Client)obj).connectionId;
            }
        }
    }
}
