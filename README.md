## AD-5370 / AD5370 / EVAL-AD5370

### Background
This codebase is for Analog Devices, AD5370 chip's evaluation board EVAL-AD5370. The code uses the default DLL and drivers that have been provided by Analog Devices in their forum. 

[https://ez.analog.com/data_converters/precision_dacs/f/q-a/27710/ad5370-ad5371-ad5372-and-ad5373-evaluation-board-software-commands/235341#235341](https://ez.analog.com/data_converters/precision_dacs/f/q-a/27710/ad5370-ad5371-ad5372-and-ad5373-evaluation-board-software-commands/235341#235341)

Although configured for AD5370, I am pretty sure it would work with AD5371,72 and 73 evaluation boards. The code is well documented and is I expect it to work with multiple evaluation boards with minimal or zero modifications. This library can connect to multiple boards, set up the firmware onto the Cyprus USB front end in the evaluation board, and initialize the SPI lines for it to work. 

### Code Performance
I have tested the timing for this board for different functions; Your mileage might vary with different configurations, but ideally should be around this. 
* writeVoltage - 1130 microseconds
* writeSPIWord - 1135 microseconds
* setLDAC - 130 microseconds

If you are looking for even higher performance, look at lightningviper: https://github.com/lightningviper/lightningstools which performs at around 850-950 microseconds for updating the equivalent for writeVoltage. 

### Basics of How AD Evaluation Boards Work
The board has a Cypress Semiconductors cy7c68013a USB chip; this provides the USB interface and backends to the SPI lines of AD5370. The default driver for this Cypress chip is called "CYUSB" which is provided by Analog Devices. This particular version of CYUSB is what is used by their evaluation software provided along with the EVAL board.  They also provide a DLL and a LIB file which provides an "API", (in quotes because it is pretty hard to find what it is). There is no associated header file, so you need to use dumpbin to find out the functions and signatures and work with it. 

The DLL helps you search for the device, once you find it, you need to connect it to get a device handle. This handle is needed for future SPI write needs. It is also important to note that once the handle is received before you proceed with any other call, ensure that the *initialize* call is called with that particular command. Then it should work.

### Warning and Notes
* Analog Devices have very scanty, even sketchy documentation from ages ago. 
* The DLL performance is limited to ~1.1ms per sample
* If you need faster sample update rate, LIBUSB-Win32 would work to around 0.85 to 0.95 ms per sample, but not significantly faster. LIBUSB-Win32 driver for EVAL-AD-5370 is best explored by https://github.com/lightningviper/lightningstools. They suggest using ZaDig https://zadig.akeo.ie/ to change and update the drivers.
* Rest of my code base should be pretty accessibly, and straightforward with plenty of documentation. 
* AD5370 is directly supported on SPI lines on Linux via Linux IIO (Industrial Input/Output) directly available in Torvalds kernel; it is available at https://github.com/torvalds/linux/blob/master/drivers/iio/dac/ad5360.c 
* This codebase is based on windows 10. And is a 32-bit application due to DLL limitations? 

If at all, you are looking to write something faster than around 0.5 ms per sample. Reach out to me. That is my current mission.
