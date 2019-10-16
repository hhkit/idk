using System.Collections;


namespace idk
{
    public class Coroutine
    {
        IEnumerator itr;
        YieldInstruction yield_instruction;

        public bool running { get; private set; } = true;

        internal Coroutine(IEnumerator enumerator)
        {
            itr = enumerator;
        }

        /// <summary>
        ///     Execute the coroutine's current frame
        /// </summary>
        /// <returns>
        ///     True if there are things left to execute
        ///     False if coroutine has ended
        /// </returns>
        internal bool executeFrame()
        {
            if (yield_instruction != null)
                yield_instruction = yield_instruction.execute();

            if (yield_instruction == null)
            {
                if (itr.MoveNext())
                {
                    yield_instruction = (YieldInstruction)itr.Current;
                    return true;
                }
                else
                    return running = false;
            }

            return true;
        }
    }
}
