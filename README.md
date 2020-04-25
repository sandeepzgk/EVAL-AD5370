## AD-5370 / AD5370 / EVAL-AD5370

### Background
This code base is developed over analog devices, AD5370 chip's evaluation board EVAL-AD5370. The code uses the default DLL and drivers that have been provided by Analog Devices in their forum. 

[https://ez.analog.com/data_converters/precision_dacs/f/q-a/27710/ad5370-ad5371-ad5372-and-ad5373-evaluation-board-software-commands/235341#235341](https://ez.analog.com/data_converters/precision_dacs/f/q-a/27710/ad5370-ad5371-ad5372-and-ad5373-evaluation-board-software-commands/235341#235341)

Although configured for AD5370, I am pretty sure it would work with AD5371,72 and 73 evaluation boards. The code is pretty well documented. And is expected to work with multiple evaluation boards. This library can connect to multiple boards, setup the firmware onto the Cyprus USB front end in the evaluation board, and initalize the SPI lines for it to work. 

### Code Performance
I have tested the timing for this board for different functions, Your milage might vary with different configurations, but ideally should be around this. 
* writeVoltage - 1130 microseconds
* writeSPIWord - 1135 microseconds
* setLDAC - 130 microseconds

If you are looking for even higher performance, for this codebase, I have so far seen this following code base, by lightningviper at [https://github.com/lightningviper/lightningstools](https://github.com/lightningviper/lightningstools) which performs at around 850-950 microseconds for updating the equvalent for writeVoltage. 

### Basics of How AD Evaluation Boards Work
The board is designed with a Cypress Semiconductors cy7c68013a usb chip, this provides the USB interface and backends to the SPI lines of AD5370. The default driver for this Cypress chip is called "CYUSB" which is provided by Analog Devices. This particular version of CYUSB is what is used by their evaulation software provided along with the EVAL board.  They also provide a DLL and a LIB file which provides an "API", (in quotes because it is pretty hard to find what it is). There is no associated header file, so you need to use dumpbin to find out the functions and signatures and work with it. 

The DLL helps you search for the device, once you find it, you need to connect it to get a device handle. This handle is used for future SPI write needs. It is also important to note that once the handle is recived before you proceed with any other call, ensure that the initalize call is called with that particular command. Then it should work

### Warning and Notes
* Analog Devices have very scanty, even sketchy documentation from ages ago. 
* The DLL performance is limited to ~1.1ms per sample
* If you need faster, LIBUSB-Win32 would work to around 0.85 to 0.95 ms per sample, but not significantly faster. LIBUSB-Win32 driver for EVAL-AD-5370 is best explored by https://github.com/lightningviper/lightningstools. 
* Rest of my code base should be pretty accessibly, and straightforward with pleanty of documentation. 
* AD5370 is directly supported on SPI lines on linux via Linux IIO (Industrial Input/Output) directly available in torvalds kernel, it can be seen at [https://github.com/torvalds/linux/blob/master/drivers/iio/dac/ad5360.c](https://github.com/torvalds/linux/blob/master/drivers/iio/dac/ad5360.c). 
* This code base is based on windows 10. And is a 32 bit application due to DLL limitations. 

If at all you are looking to write something faster than around 0.5 ms per sample. Reach out to me. That is my current mission.
