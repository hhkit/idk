namespace idk
{
    public struct Collision
    {
        internal ulong colliderID;
        internal Vector3 _normal;
        internal Vector3 _point;

        public Collider collider { get 
            { 
                var ret = new Collider();
                ret.handle = colliderID; 
                return ret; 
            }
        }
        public GameObject gameObject { get => collider.gameObject; } 
        public RigidBody rigidBody { get => collider.attachedRigidbody; } 
        public Transform transform { get => collider.transform; }
        public Vector3 normal { get => _normal; } 
        public Vector3 contactPoint { get => _point; }
    }
}
