using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace X10Interface
{
    public class FunctionCode
    {
        public FunctionCode(string name, bool[] bitCode)
        {
            Name = name;
            BitCode = bitCode;
        }

        public string Name { get; }
        public bool[] BitCode { get; }

        public byte GetByte()
        {
            byte tempByte = 0;
            for (int i = 0; i < BitCode.Length; i++)
            {
                if (BitCode[i])
                    tempByte |= (byte)(1 << BitCode.Length - 1 - i);
                else
                    tempByte |= (byte)(0 << BitCode.Length - 1 - i);
            }

            return tempByte;
        }
    }
}