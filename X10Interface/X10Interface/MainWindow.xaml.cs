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
        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
        }

        public List<string> SerialPorts { get; } = new List<string>(SerialPort.GetPortNames().Any() ? SerialPort.GetPortNames() : new[] { "None Connected" });
        public List<int> BaudRates { get; } = new List<int>()
        {
            110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200
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
            new HouseCode("HOUSE_A", new[]{false,true,true,false, }),
            new HouseCode("HOUSE_B", new []{false,true,true,true,}),
            new HouseCode("HOUSE_C", new []{false,true,false,false,}),
            new HouseCode("HOUSE_D", new []{true,false,false,false,}),
            new HouseCode("HOUSE_E", new []{false,true,false,true,}),
            new HouseCode("HOUSE_F", new []{true,false,false,true,}),
            new HouseCode("HOUSE_G", new []{true,false,true,false,}),
            new HouseCode("HOUSE_H", new []{true,false,true,true,}),
            new HouseCode("HOUSE_I", new []{true,true,true,false,}),
            new HouseCode("HOUSE_J", new []{true,true,true,true,}),
            new HouseCode("HOUSE_K", new []{true,true,false,false,}),
            new HouseCode("HOUSE_L", new []{true,true,false,true,}),
            new HouseCode("HOUSE_M", new []{false,false,false,false,}),
            new HouseCode("HOUSE_N", new []{false,false,false,true,}),
            new HouseCode("HOUSE_O", new []{false,false,true,false,}),
            new HouseCode("HOUSE_P", new []{false,false,true,true,}),
        };

        public List<KeyCode> KeyCodes { get; set; } = new List<KeyCode>()
        {
            new KeyCode("KEY_1", new[] { false, false, true, true, false, }),
            new KeyCode("KEY_2", new[] { false, false, true, true, true, }),
            new KeyCode("KEY_3", new[] { false, false, true, false, false, }),
            new KeyCode("KEY_4", new[] { false, false, true, false, true, }),
            new KeyCode("KEY_5", new[] { false, true, false, false, false, }),
            new KeyCode("KEY_6", new[] { false, true, false, false, true, }),
            new KeyCode("KEY_7", new[] { false, true, false, true, false, }),
            new KeyCode("KEY_8", new[] { false, true, false, true, true, }),
            new KeyCode("KEY_9", new[] { false, true, true, true, false, }),
            new KeyCode("KEY_10", new[] { false, true, true, true, true, }),
            new KeyCode("KEY_11", new[] { false, true, true, false, false, }),
            new KeyCode("KEY_12", new[] { false, true, true, false, true, }),
            new KeyCode("KEY_13", new[] { false, false, false, false, false, }),
            new KeyCode("KEY_14", new[] { false, false, false, false, true, }),
            new KeyCode("KEY_15", new[] { false, false, false, true, false, }),
            new KeyCode("KEY_16", new[] { false, false, false, true, true, }),
        };


        public List<FunctionCode> FunctionCodes { get; set; } = new List<FunctionCode>()
        {
            new FunctionCode("ALL_UNITS_OFF", new[] {true,false,false,false,false,}),
            new FunctionCode("ALL_LIGHTS_ON", new[] {true,true,false,false,false,}),
            new FunctionCode("ON",  new[] {true,false,true,false,false,}),
            new FunctionCode("OFF",  new[] {true,true,true,false,false,}),
            new FunctionCode("DIM",  new[] {true,false,false,true,false,}),
            new FunctionCode("BRIGHT",  new[] {true,true,false,true,false,}),
            new FunctionCode("ALL_LIGHTS_OFF",  new[] {true,false,true,true,false,}),
            new FunctionCode("EXTENDED_CODE", new[] {true,true,true,true,false,}),
            new FunctionCode("HAIL_REQUEST", new[] {true,false,false,false,true,}),
            new FunctionCode("HAIL_ACKNOWLEDGE", new[] {true,true,false,false,true,}),
            new FunctionCode("PRE_SET_DIM",  new[] {true,false,true,false,true,}),
            new FunctionCode("EXTENDED_DATA", new[] {true,false,false,true,true,}),
            new FunctionCode("STATUS_ON", new[] {true,true,false,true,true,}),
            new FunctionCode("STATUS_OFF",  new[] {true,false,true,true,true,}),
            new FunctionCode("STATUS_REQUEST",  new[] {true,true,true,true,true,}),
        };


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var serialPortName = SerialListBox.SelectedItem.ToString();
            var baudRate = int.Parse(BuadRateListBox.SelectedItem.ToString());

            var bytesToSend = new[] { SelectedHouseCode.GetByte(), SelectedKeyCode.GetByte(), SelectedFunctionCode.GetByte() };


            if (serialPortName != "None Connected")
            {
                var serialPort = new SerialPort(serialPortName, baudRate);
                serialPort.Write(bytesToSend, 0, bytesToSend.Count());
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
