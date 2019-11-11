using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class RaycastHit
    {
        internal ulong collider_id;
        internal Vector3 pt;
        internal float dist;
        
        public Collider collider
        {
            get
            {
                var col = new Collider();
                col.handle = collider_id;
                return col;
            }
        }

        public RigidBody rigidbody
        {
            get => collider.attachedRigidbody;
        }

        public Transform transform
        {
            get => collider.transform;
        }

        public Vector3 point
        {
            get => pt;
            internal set => pt = value;
        }

        public float distance
        {
            get => dist;
            internal set => dist = value;
        }
    }
}
