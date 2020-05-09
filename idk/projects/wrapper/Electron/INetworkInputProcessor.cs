namespace idk
{
    public interface INetworkInputProcessor<T>
    {
        T GenerateInput();
        void ProcessInput(T input);
    }
}
