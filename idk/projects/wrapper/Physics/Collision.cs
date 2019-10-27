namespace idk
{
    public struct Collision
    {
        internal ulong colliderID;
        internal Vector3 _impulse;
        internal Vector3 _relativeVelocity;

        public Collider collider { get 
            { 
                var ret = new Collider();
                ret.handle = colliderID; 
                return ret; 
            }
        }
        public GameObject gameObject { get { return collider.gameObject; } }
        public RigidBody rigidBody { get { return collider.attachedRigidbody; } }
        public Vector3 relativeVelocity { get { return _relativeVelocity; } }
        public Vector3 impulse { get { return _impulse; } }
    }
}
