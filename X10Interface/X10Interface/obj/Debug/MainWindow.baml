   M S B A M L   `   `   `  ����G  CX10Interface, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null+clr-namespace:X10InterfaceX10Interface  5   	       X10Interface.MainWindow   5   	   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   @d2http://schemas.microsoft.com/expression/blend/2008     5   	   Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5   	   ,localclr-namespace:X10Interface     5   	     � Title$  
MainWindow��5   	   $	��450��6   $	��800��6)    � 	Resources 5	      )  #System.Windows.Controls.DataGridRow �� 
TargetType   DatagridRowStyle �� Triggers P� Property P� Value �� Property �� Value*  $System.Windows.Controls.DataGridCell  DatagridCellStyle��%J  &	        &	 �     �� 5
   
   "  6    5      P� 5       �@ 
IsSelected . . �5      ��  � ��
 �����63   �� 5       � ��
 �����63   5      5      �� 5   
   "  6    5      P� 5       �@ 
IsSelected . . �5      ��  � ��
 �����63   5      5      5      .��5      �  � ColumnDefinitions 5   
   �� 5      �� 5      �� 5      	 � RowDefinitions	 5    
   �� 5!      $
��Auto �6   �� 5"      $
��Auto �6   �� 5#      $��* �6   �� 5$      $
��Auto �6   .+�5(   
   �� $��0��6   $��1��6   $��Center=�6-   $��18(�6J   .��6X   MOMS controlpanel6k   �� 5*   
   $��
10 0 10 10q�6   $��1��6)   $��0��66   .+�5+      �� $��Center=�6   .��61   
SerialPort6=   �� 5,      -   5        
 ��Name$
 SerialListBox��5,       J� 	AllowDrop . $ �� IsSynchronizedWithCurrentItem$
 TrueY�6c    �� SelectedIndex$ 0��6�   ��6.   �� *SerialPorts+6�   5-      �� 5/   
   $��
10 0 10 10q�6   $��1��6)   $��1��66   .+�50      �� $��Center=�6   .��61   
BaudRate6;   �� 51      -   5        $
 BuadRateListBox��51       . $
 TrueY�6c   $ 0��6�   ��60   �� *	BaudRates+6�   52      &   System.Windows.Controls.DataGrid  54   
   -   5          Name$ 	HouseCode��54   �      AutoGenerateColumns .  $��2��6�   $��0��6�    �� SelectedItem 6   �� *SelectedHouseCode+   RowStyle 6>   ��  �� Source# [     	CellStyle 6|   �� # [ ��6  �� *DataContext.HouseCodes+   Columns 55      0  *System.Windows.Controls.DataGridTextColumn  56      ,  &System.Windows.Controls.DataGridColumn   Header$ 	HouseCode��6%      Width5  /System.Windows.Controls.DataGridLengthConverter$ * 6V   1  +System.Windows.Controls.DataGridBoundColumn   Binding 68   ��  �� Path$
 Name�6V   58        59   
   -   5        $ KeyCode��59   �    .  $��2��6�   $��1��6�    6   �� *SelectedKeyCode+ 6<   �� # [   6z   �� # [ ��6  �� *
KeyCodes+ 5:        5;      $ KeyCode��6%   $ * 6T    66   �� $
 Name�6T   5=        5>   
   -   5        $ FunctionCode��5>   �    .  $��2��6�   $��2��6�    6   �� *SelectedFunctionCode+ 6A   �� # [   6   �� # [ ��6  �� *FunctionCodes+ 5?        5@      $ FunctionCode��6%   $ * 6Y    6;   �� $
 Name�6Y   5B      �� 5D   
   -   5        $��3��5D      $��2��6   $��35��6.   $��10q�6:   {�6[   �� *
CanClick+.��6z   Send Command6�   5E      5F      