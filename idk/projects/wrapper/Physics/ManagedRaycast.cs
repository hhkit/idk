using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    internal struct ManagedRaycast
    {
        public bool valid;
        public ulong collider_id;
        public float distance;
        public Vector3 normal;
        public Vector3 point_of_collision;
    }
}
