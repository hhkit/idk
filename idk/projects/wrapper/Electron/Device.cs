namespace idk
{
    public class Device
    {
        internal string mac_addr;

        public Address[] addresses { get => Bindings.NetworkDeviceGetAddresses(mac_addr); }
        public ushort subnet_bits;
    }
}
