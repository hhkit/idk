namespace idk
{
    public class Player
    {
        int connectionId;
        public int ActorNumber { get => connectionId; }
        internal Player(int i)
        {
            connectionId = i;
        }
    }
}
