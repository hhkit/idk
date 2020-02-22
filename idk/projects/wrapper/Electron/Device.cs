namespace idk
{
    public class Device
    {
        internal string _name;
        internal string _fullname;

        string name { get => _name; }
        string fullName { get => _fullname; }

        public Address[] addresses { get => Bindings.NetworkDeviceGetAddresses(name); }
    }
}
