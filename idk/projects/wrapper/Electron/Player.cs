namespace idk
{
    public class Player
    {
        public int ActorNumber { get; internal set; }
        internal Player(int i)
        {
            ActorNumber = i;
        }
    }
}
