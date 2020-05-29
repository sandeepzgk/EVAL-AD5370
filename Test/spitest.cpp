/***
Sandeep Zechariah George Kollannur 
NOTE: This code is for RaspberryPi 3, tested only on RPi3, with Raspbian 10 (buster) / gcc version 8.3.0 (Raspbian 8.3.0-6+rpi1)
RaspberryPi Performance Testing for AD5370, direct control via RPi 3 B+
Update Frequency Achieved - 7.7 kHz, with +1, -1 volt on one channel
### Running Instruction: g++ spitest.cpp -lwiringPi -lgpiod -o spitest && ./spitest
### Code Based on https://github.com/sparkfun/Pi_Wedge/blob/master/software/spitest.cpp 
### Strace to figure out system calls
### g++ spitest.cpp -lwiringPi -lgpiod -o spitest && strace ./spitest 2>&1 | grep "(3," not needed
### during my testing file descriptor 3 was for GPIO and 4 was for SPI, it will change based on system used.

**** Packages Installed (May not be a complete list****
sudo apt-get install spi-tools
sudo apt-get install pinout
sudo apt-get install gpio
sudo apt-get install wiringpi
  
***/
#include <iostream>
#include <errno.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <unistd.h>

using namespace std;

int main()
{
	
	///  Use command '''gpio readall''' to get Pin Mapping Information from Raspberry Pi
	const int LDAC_pin  = 0; //BCMGPIO 17 | WiringPi 0 | Physical 11
	const int BUSY_pin  = 3; //BCMGPIO 22 | WiringPi 3 | Physical 15
	const int CLR_pin   = 2; //BCMGPIO 27 | WiringPi 2 | Physical 13
	const int RESET_pin = 7; //BCMGPIO 4  | WiringPi 7 | Physical 7

	wiringPiSetup(); //For GPIO Setup

	pinMode(LDAC_pin , OUTPUT);    //Setup AD5370 Pins
	pinMode(BUSY_pin , INPUT);     //Setup AD5370 Pins
	pinMode(CLR_pin  , OUTPUT);    //Setup AD5370 Pins
	pinMode(RESET_pin, OUTPUT);    //Setup AD5370 Pins
   
	// channel is the wiringPiSPI name for the chip select (or chip enable) pin.
	// Set this to 0 or 1, depending on how it's connected.
	static const int CHANNEL = 0;
   
	int fd, result;
	unsigned char buffer[100];

	// Configure the wiringPiSPI interface.
	// CHANNEL insicates chip select,
	// 500000 indicates bus speed.
	fd = wiringPiSPISetupMode(CHANNEL, 500000,3);  //Returns the File Descriptor for SPI

	cout << "wiringPiSPISetupMode Init result: " << fd << endl;

	//Perform a Reset Activity
	digitalWrite(RESET_pin, LOW);
	usleep(1);
	digitalWrite(RESET_pin, HIGH);
	
	//Perform a Clear Activity
	digitalWrite(CLR_pin, LOW);
	usleep(1);
	digitalWrite(CLR_pin, HIGH);
	
	//Set LDAC_pin HIGH for continious update write
	digitalWrite(LDAC_pin, HIGH);

	for(int i=0;i<100000;i++)
	{
		//[201, 98, 32] (for +1 V) , 201, is channel 1, 200, is channel 0, 203, is channel 3
		buffer[0] = 203;
		buffer[1] = 98;
		buffer[2] = 32;
		if(digitalRead(BUSY_pin) == HIGH) //Ensuring the system (AD5370) is not busy so that we can write to the register, not important for one channel, but might be important for multi channel
			wiringPiSPIDataRW(CHANNEL, buffer, 3); //The word has to be written simultaneously, the buffer is overwritten by the read activity (RW), after the write.

		//usleep(1); //Sleep to stabilize the loop, so far we have achieved 7.7 kHz, but it flickers around 7.5 to 7.8 kHz, to stabilize the loop, the usleep timer is used, may be more precision timers may be required for the future.
		
		//[201, 72, 135]   (for -1 V)		
		buffer[0] = 203;
		buffer[1] = 72;
		buffer[2] = 135;
		if(digitalRead(BUSY_pin) == HIGH)
			wiringPiSPIDataRW(CHANNEL, buffer, 3);
		
		//usleep(1);
	}
}