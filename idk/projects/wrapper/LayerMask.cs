using System.Runtime.InteropServices;

namespace idk
{
    [StructLayout(LayoutKind.Sequential)]
    public struct LayerMask
    {
        public int value;

        public static implicit operator LayerMask(int value) => value;
        public static implicit operator int(LayerMask mask) => mask.value;
        
        public static LayerMask GetMask(params string[] layerNames)
        {
            int mask = 0;
            foreach (var name in layerNames)
            {
                int index = NameToLayer(name);
                if(index >= 0)
                    mask |= (1 << index);
            }
            return mask;
        }

        public static string LayerToName(int layerIndex)
            => Bindings.LayerMaskLayerToName(layerIndex);

        public static int NameToLayer(string layerName)
            => Bindings.LayerMaskNameToLayer(layerName);
    }
}
