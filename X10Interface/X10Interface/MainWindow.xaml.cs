using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using System.IO.Ports;


namespace X10Interface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        private const string NoSerialPortFound = "No connected devices";

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
        }

        public List<string> SerialPorts { get; } = new List<string>(SerialPort.GetPortNames().Any() ? SerialPort.GetPortNames() : new[] { NoSerialPortFound });
        public List<int> BaudRates { get; } = new List<int>()
        {
            300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200
        };

        private HouseCode selectedHouseCode;
        private KeyCode selectedKeyCode;
        private FunctionCode selectedFunctionCode;
        public HouseCode SelectedHouseCode
        {
            get => selectedHouseCode;
            set
            {
                selectedHouseCode = value;
                OnPropertyChanged("CanClick");
            }
        }
        public KeyCode SelectedKeyCode
        {
            get => selectedKeyCode;
            set
            {
                selectedKeyCode = value;
                OnPropertyChanged("CanClick");
            }
        }
        public FunctionCode SelectedFunctionCode
        {
            get => selectedFunctionCode;
            set
            {
                selectedFunctionCode = value;
                OnPropertyChanged("CanClick");
            }
        }
        public bool CanClick => SelectedHouseCode != null && SelectedKeyCode != null && SelectedFunctionCode != null;

        public List<HouseCode> HouseCodes { get; set; } = new List<HouseCode>()
        {
            new HouseCode("HOUSE_A", 0B0110),
            new HouseCode("HOUSE_B", 0B0111),
            new HouseCode("HOUSE_C", 0B0100),
            new HouseCode("HOUSE_D", 0B1000),
            new HouseCode("HOUSE_E", 0B0101),
            new HouseCode("HOUSE_F", 0B1001),
            new HouseCode("HOUSE_G", 0B1010),
            new HouseCode("HOUSE_H", 0B1011),
            new HouseCode("HOUSE_I", 0B1110),
            new HouseCode("HOUSE_J", 0B1111),
            new HouseCode("HOUSE_K", 0B1100),
            new HouseCode("HOUSE_L", 0B1101),
            new HouseCode("HOUSE_M", 0B0000),
            new HouseCode("HOUSE_N", 0B0001),
            new HouseCode("HOUSE_O", 0B0010),
            new HouseCode("HOUSE_P", 0B0011),
        };

        public List<KeyCode> KeyCodes { get; set; } = new List<KeyCode>()
        {
            new KeyCode("KEY_1", 0B00110),
            new KeyCode("KEY_2", 0B00111),
            new KeyCode("KEY_3", 0B00100),
            new KeyCode("KEY_4", 0B00101),
            new KeyCode("KEY_5", 0B01000),
            new KeyCode("KEY_6", 0B01001),
            new KeyCode("KEY_7", 0B01010),
            new KeyCode("KEY_8", 0B01011),
            new KeyCode("KEY_9", 0B01110),
            new KeyCode("KEY_10", 0B01111),
            new KeyCode("KEY_11", 0B01100),
            new KeyCode("KEY_12", 0B01101),
            new KeyCode("KEY_13", 0B00000),
            new KeyCode("KEY_14", 0B00001),
            new KeyCode("KEY_15", 0B00010),
            new KeyCode("KEY_16", 0B00011),
        };


        public List<FunctionCode> FunctionCodes { get; set; } = new List<FunctionCode>()
        {
            new FunctionCode("ALL_UNITS_OFF", 0B10000),
            new FunctionCode("ALL_LIGHTS_ON", 0B11000),
            new FunctionCode("ON", 0B10100),
            new FunctionCode("OFF", 0B11100),
            new FunctionCode("DIM", 0B10010),
            new FunctionCode("BRIGHT", 0B11010),
            new FunctionCode("ALL_LIGHTS_OFF", 0B10110),
            new FunctionCode("EXTENDED_CODE", 0B11110),
            new FunctionCode("HAIL_REQUEST", 0B10001),
            new FunctionCode("HAIL_ACKNOWLEDGE", 0B11001),
            new FunctionCode("PRE_SET_DIM", 0B10101),
            new FunctionCode("EXTENDED_DATA", 0B10011),
            new FunctionCode("STATUS_ON", 0B11011),
            new FunctionCode("STATUS_OFF", 0B10111),
            new FunctionCode("STATUS_REQUEST", 0B11111),
        };


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var serialPortName = SerialListBox.SelectedItem.ToString();
            var baudRate = int.Parse(BuadRateListBox.SelectedItem.ToString());

            var bytesToSend = new[] { SelectedHouseCode.ByteCode, SelectedKeyCode.ByteCode, SelectedFunctionCode.ByteCode };


            if (serialPortName != NoSerialPortFound)
            {
                var serialPort = new SerialPort(serialPortName.Trim(), baudRate)
                {
                    // Set if needed.
                    //Parity = Parity.None,
                    //StopBits = StopBits.None,
                    StopBits = StopBits.One,
                    //RtsEnable = true,
                    DataBits = 8,
                    Handshake = Handshake.None,
                };
                serialPort.Open();
                //serialPort.WriteLine("hello");
                serialPort.Write(bytesToSend, 0, 3);
                serialPort.Close();
                }
        }



        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
