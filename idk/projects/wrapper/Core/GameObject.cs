using System;

namespace idk
{
    public class GameObject
        : Object
    {
        public bool activeSelf { get => Bindings.GameObjectActiveSelf(handle); }
        public bool activeInHierarchy { get => Bindings.GameObjectGetActiveInHierarchy(handle); }
        public Transform transform { get { return GetComponent<Transform>(); } }
        public string name { get => Bindings.GameObjectGetName(handle); set => Bindings.GameObjectSetName(handle, value); }
        public string tag { get => Bindings.GameObjectGetTag(handle); set => Bindings.GameObjectSetTag(handle, value); }
        public int layer { get => Bindings.GameObjectGetLayer(handle); set => Bindings.GameObjectSetLayer(handle, value); }

        public GameObject()
        {
            handle = Bindings.GameObjectNew();
        }

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
            {
                var component = Bindings.GameObjectAddGameComponent(handle, typeof(T).Name) as T;
                // else try to find corresponding monobehavior
                return component;
            }
        }


        public T GetComponent<T>() where T : Component, new()
        {
            ulong comp_handle = Bindings.GameObjectGetEngineComponent(handle, typeof(T).Name);

            if (comp_handle != 0)
            {
                T component = new T();
                component.handle = comp_handle;
                return component;
            }
            else
            {
                var component = Bindings.GameObjectGetGameComponent(handle, typeof(T).Name) as T;
                // else try to find corresponding monobehavior
                return component;
            }

        }

        public T GetComponentInChildren<T>() where T : Component, new()
        {
            var children = transform.GetChildren();
            foreach (var child in children)
            {
                var comp = child.GetComponent<T>();
                if (comp)
                    return comp;
            }

            foreach (var child in children)
            {
                var comp = child.GetComponentInChildren<T>();
                if (comp)
                    return comp;
            }

            return null;
        }

        public T GetComponentInParent<T>() where T: Component, new()
        {
            var comp = GetComponent<T>();
            if (comp)
                return comp;

            var parent = transform.parent;
            if (parent)
                return parent.GetComponent<T>();
            else
                return null;
        }

        public void SetActive(bool new_active)
            => Bindings.GameObjectSetActive(handle, new_active);


        // statics
        public static GameObject FindWithTag(string tag)
        {
            var id = Bindings.GameObjectFindWithTag(tag);
            return id != 0 ? new GameObject(id) : null;
        }
        public static GameObject[] FindGameObjectsWithTag(string tag)
        {
            return Bindings.GameObjectFindGameObjectsWithTag(tag);
        }
    }
}
