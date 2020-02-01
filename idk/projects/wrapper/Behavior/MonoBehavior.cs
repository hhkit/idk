using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

namespace idk
{
    public class MonoBehavior : Component
    {
        List<Coroutine> coroutines = new List<Coroutine>();
        List<Coroutine> new_coroutines = new List<Coroutine>();

        public bool enabled
        {
            get => Bindings.MonoBehaviorGetEnable(handle);
            set => Bindings.MonoBehaviorSetEnable(handle, value);
        }

        /// public virtual void OnCollisionEnter(Collision other) { }
        /// public virtual void OnCollisionStay(Collision other) { }
        /// public virtual void OnCollisionExit(Collision other) { }

        /// <summary>
        /// Start a coroutine.
        /// </summary>
        /// <param name="enumerator">Return from IEnumerator function</param>
        /// <returns>Reference to the coroutine.</returns>
        /// <example>
        /// <code>
        /// class Potato : MonoBehavior
        /// {
        ///     private IEnumerator MyCoroutine()
        ///     {
        ///         Debug.Log("Yolo");
        ///         yield return new WaitForSeconds(5f);   
        ///     }
        ///     
        ///     void Start()
        ///     {
        ///         StartCoroutine(MyCoroutine());
        ///     }
        /// }
        /// </code>
        /// </example>
        public Coroutine StartCoroutine(IEnumerator enumerator)
        {
            var instr = enumerator.Current;
            var coroutine = new Coroutine(enumerator);
            new_coroutines.Add(coroutine);
            return coroutine;
        }
        public override Object Clone()
        {
            var retval = (MonoBehavior) MemberwiseClone();
            retval.coroutines = new List<Coroutine>();
            retval.new_coroutines = new List<Coroutine>();
            return retval;
        }

        internal void UpdateCoroutines()
        {
            foreach (var coro in coroutines)
                coro.executeFrame();

            foreach (var coro in new_coroutines)
                coroutines.Add(coro);

            new_coroutines.Clear();

            coroutines.RemoveAll(delegate (Coroutine coro) { return !coro.running; });
        }

        /// <summary>
        ///     Stop a coroutine.
        /// </summary>
        /// <param name="stopme">Reference to coroutine</param>
        public void StopCoroutine(Coroutine stopme)
        {
            coroutines.Remove(stopme);
        }

    }
}
