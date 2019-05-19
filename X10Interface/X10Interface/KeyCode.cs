using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace X10Interface
{
    public class KeyCode
    {
        public KeyCode(string name, byte byteCode)
        {
            Name = name;
            ByteCode = byteCode;
        }

        public string Name { get; }
        public byte ByteCode { get; }
    }
}