using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class Physics
    {
        public const int DefaultRaycastLayers = ~(1 << 1);

        public static bool Raycast(Vector3 origin, Vector3 direction, float max_dist = Mathf.Infinity, int mask = DefaultRaycastLayers, bool queriesHitTriggers = false)
        {
            var result = Bindings.PhysicsRaycast(origin, direction, max_dist, mask, queriesHitTriggers);
            return result.valid;
        }

        public static bool Raycast(Vector3 origin, Vector3 direction, out RaycastHit hitInfo, float max_dist = Mathf.Infinity, int mask = DefaultRaycastLayers, bool queriesHitTriggers = false)
        {
            var result = Bindings.PhysicsRaycast(origin, direction, max_dist, mask, queriesHitTriggers);
            hitInfo = new RaycastHit();
            hitInfo.collider_id = result.collider_id;
            hitInfo.pt = result.point_of_collision;
            hitInfo.dist = result.distance;
            return result.valid;
        }
    }
}
