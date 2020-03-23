using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class ElectronTransformView
        : Component
    {
        public Vector3 position
        {
            get => transform.position;
            set => Bindings.ElectronTransformSetPosition(handle, value);
        }

        public Quaternion rotation
        {
            get => transform.rotation;
            set => Bindings.ElectronTransformSetRotation(handle, value);
        }

        public Vector3 scale
        {
            get => transform.scale;
            set => Bindings.ElectronTransformSetScale(handle, value);
        }

        public void Translate(Vector3 translation)
        {
            Bindings.ElectronTransformTranslate(handle, translation);
        }

        public void Rotate(Quaternion rotation)
        {
            Bindings.ElectronTransformRotate(handle, rotation);
        }
    }
}
