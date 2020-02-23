namespace idk
{
    /// <summary>
    /// An identifier for a player on the network.
    /// </summary>
    public class Player
    {
        /// <summary>
        /// The Server's ActorNumber is hardcoded to -1.
        /// </summary>
        public static int ServerId { get => -1; }
        public static Player Server { get => new Player(ServerId); }

        int connectionId;

        /// <summary>
        /// The Network Identifier for the player.
        /// Values range from 0 to 3 for up to 4 conections. -1 corresponds to the Server.
        /// </summary>
        public int ActorNumber { get => connectionId; }

        internal Player(int i)
        {
            connectionId = i;
        }

        public override string ToString()
        {
            if (connectionId == ServerId)
                return "Server";
            else
                return "Client " + connectionId;
        }

        public static bool operator ==(Player lhs, Player rhs)
        {
            if ((object) lhs != null && (object) rhs != null)
                return lhs.connectionId == rhs.connectionId;
            else
                return (object)lhs == null && (object)rhs == null;
        }

        public static bool operator !=(Player lhs, Player rhs)
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
                return this.connectionId == ((Player)obj).connectionId;
            }
        }
    }
}
