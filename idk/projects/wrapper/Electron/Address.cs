namespace idk
{
    public struct Address
    {
        public byte a, b, c, d;
        public short port;

        public Address(int _a, int _b, int _c, int _d)
        {
            a = (byte) _a;
            b = (byte) _b;
            c = (byte) _c;
            d = (byte) _d;
            port = 0;
        }
    }
}
