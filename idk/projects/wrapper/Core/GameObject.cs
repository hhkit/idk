using System;

namespace idk
{
    public class GameObject
        : Object
    {
        public bool activeSelf { get => Bindings.GameObjectActiveSelf(handle); }
        public bool activeInHierarchy { get => Bindings.GameObjectGetActiveInHierarchy(handle); }
        public Transform transform { get { return GetComponent<Transform>(); } }

        internal GameObject(ulong in_handle)
        {
            handle = in_handle;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            var comp_handle = Bindings.GameObjectAddEngineComponent(handle, typeof(T).Name);
            if (comp_handle != 0)
            {
                T component = new T();
                component.handle = comp_handle;
                return component;
            }
            else
                // else try to find corresponding monobehavior
                return null;
        }

        public T GetComponent<T>() where T : Component, new() 
        {
            var comp_handle = Bindings.GameObjectGetEngineComponent(handle, typeof(T).Name);

            if (comp_handle != 0)
            {
                T component = new T();
                component.handle = comp_handle;
                return component;
            }
            else
                // else try to find corresponding monobehavior
                return null;
        }

        public void SetActive(bool new_active)
        {
            Bindings.GameObjectSetActive(handle, new_active);
        }

    }
}
