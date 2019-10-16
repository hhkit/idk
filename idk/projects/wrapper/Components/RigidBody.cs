namespace idk
{
    public class RigidBody
        : Component
    {
        public float mass { 
            get => Bindings.RigidBodyGetMass(handle); 
            set => Bindings.RigidBodySetMass(handle, value); 
        }

        public Vector3 velocity
        {
            get => Bindings.RigidBodyGetVelocity(handle);
            set => Bindings.RigidBodySetVelocity(handle, value);
        }

        public Vector3 position
        {
            get => Bindings.RigidBodyGetPosition(handle);
            set => Bindings.RigidBodySetPosition(handle, value);
        }

        // returns true because we don't have angular velocity ;3
        public bool freezeRotation
        {
            get => true;
        }

        public bool useGravity
        {
            get => Bindings.RigidBodyGetUseGravity(handle);
            set => Bindings.RigidBodySetUseGravity(handle, value);
        }

        public void Sleep()
            => Bindings.RigidBodySleep(handle);

        public void AddForce(Vector3 force) 
            => Bindings.RigidBodyAddForce(handle, force);

        public void Teleport(Vector3 translation) 
            => Bindings.RigidBodyTeleport(handle, translation);
    }
}
