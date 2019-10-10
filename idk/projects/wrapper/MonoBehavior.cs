﻿using System.Collections;
using System.Collections.Generic;

namespace idk
{
    public class MonoBehavior : Component
    {
        List<Coroutine> coroutines = new List<Coroutine>();
        List<Coroutine> new_coroutines = new List<Coroutine>();

        /// <summary>
        ///     Called when script starts
        /// </summary>
        public virtual void Start() { }

        /// <summary>
        ///     Called every frame
        /// </summary>
        public virtual void Update() { }

        /// <summary>
        ///     Called when script is destroyed
        /// </summary>
        public virtual void Stop() { }

        /// <summary>
        ///     Called on fixed update
        /// </summary>
        public virtual void FixedUpdate() { }

        /// <summary>
        ///     Called on fixed update
        /// </summary>
        public virtual void LateUpdate() { }

        //public virtual void OnTriggerEnter(Collider other) { };
        //public virtual void OnTriggerStay(Collider other) { };
        //public virtual void OnTriggerExit(Collider other) { };

        /// <summary>
        /// Start a coroutine.
        /// </summary>
        /// <param name="enumerator">Return from IEnumerator function</param>
        /// <returns>Reference to the coroutine.</returns>
        public Coroutine StartCoroutine(IEnumerator enumerator)
        {
            var instr = enumerator.Current;
            var coroutine = new Coroutine(enumerator);
            new_coroutines.Add(coroutine);
            return coroutine;
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
