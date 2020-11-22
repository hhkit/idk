namespace idk
{
    /// <summary>
    /// An identifier for a player on the network.
    /// </summary>
    public struct Client
    {
        int connectionId;

        /// <summary>
        /// The Network Identifier for the player.
        /// Values range from 0 to 2 for up to 3 connections. -1 corresponds to the Server.
        /// </summary>
        public int actorNumber { get => connectionId; }

        /// <summary>
        /// The Network Identifier for the player.
        /// Values range from 0 to 2 for up to 3 connections. -1 corresponds to the Server.
        /// </summary>
        public int lobbyIndex => Bindings.NetworkClientLobbyIndex(connectionId);

        public string name => Bindings.NetworkClientName(connectionId);

        internal Client(int i)
        {
            connectionId = i;
        }

        public void Evict() => Bindings.NetworkEvictClient(connectionId);

        public override string ToString()
        {
            return "Client " + actorNumber;
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
