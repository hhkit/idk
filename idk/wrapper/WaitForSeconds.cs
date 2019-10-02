﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace idk
{
    public class WaitForSeconds 
        : YieldInstruction
    {
        public double dur;

        /// <summary>
        /// Constructor for WaitForSeconds instruction.
        /// </summary>
        /// <param name="duration">Duration in SECONDS.</param>
        public WaitForSeconds(double duration)
        {
            dur = duration;
        }

        internal override YieldInstruction execute()
        {
            dur -= Time.deltaTime;

            if (dur > 0)
                return this;
            else
                return null;
        }
    }
}