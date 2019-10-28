namespace idk
{
    public class Prefab
        : Resource
    {
        public GameObject Instantiate()
        {
            var ret = Bindings.PrefabInstantiate(guid);
            return ret != 0 ? new GameObject(ret) : null;
        }
        public GameObject Instantiate(Transform parent, bool instantiateInWorldSpace = false)
        {
            var ret = Instantiate();
            if (ret)
                ret.transform.SetParent(parent, instantiateInWorldSpace);
            return ret;
        }

        public GameObject Instantiate(Vector3 position)
        {
            var ret = Instantiate();
            if (ret)
                ret.transform.position = position;

            return ret;
        }
        public GameObject Instantiate(Vector3 position, Quaternion rotation)
        {
            var ret = Instantiate(position);
            if (ret)
                ret.transform.rotation = rotation;
            return ret;
        }
    }
}
