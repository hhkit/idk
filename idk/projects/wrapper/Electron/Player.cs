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
        public static int ServerNumber { get => -1; }

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
            if (connectionId == ServerNumber)
                return "Server";
            else
                return "Client " + connectionId;
        }
    }
}
