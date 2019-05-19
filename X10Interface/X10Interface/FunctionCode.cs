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
        public FunctionCode(string name, byte byteCode)
        {
            Name = name;
            ByteCode = byteCode;
        }

        public string Name { get; }
        public byte ByteCode { get; }
    }
}