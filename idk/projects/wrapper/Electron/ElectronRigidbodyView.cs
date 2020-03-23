
namespace idk
{
    public class ElectronRigidbodyView
        : Component
    {
        public RigidBody rigidBody => GetComponent<RigidBody>();

        public Vector3 velocity
        {
            get => rigidBody.velocity;
            set => Bindings.ElectronRigidBodySetVelocity(handle, value);
        }

        public void AddForce(Vector3 force)
        {
            Bindings.ElectronRigidBodyAddForce(handle, force);
        }
    }
}
