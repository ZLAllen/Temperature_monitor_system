# Temperature_monitor_system
(Embedded C) Temperature_monitoring for electric car

This system is designed to read and display data from multiple sensors located in different parts of the electric cars.
Sensors channel selection will be done by the programmable anolog mux. Then the data from the selected channel will be converted
to Digital signed by the A2D converter and then received by the microcontroller through SPI. The MCU will convert the raw data into
actual temperature and then transmit the data for display in the LCD screen. User can control the system via buttons and a rotray encoder.
See the documents for more informations. 
