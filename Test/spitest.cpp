/***
Sandeep Zechariah George Kollannur 
NOTE: This code is for RaspberryPi 3, tested only on RPi3, with Raspbian 10 (buster) / gcc version 8.3.0 (Raspbian 8.3.0-6+rpi1)
RaspberryPi Performance Testing for AD5370, direct control via RPi 3 B+
Update Frequency Achieved - 7.7 kHz to 8 kHz, with +1, -1 volt on one channel
### Running Instruction: g++ spitest.cpp -lwiringPi -lgpiod -o spitest && ./spitest
### Code Based on https://github.com/sparkfun/Pi_Wedge/blob/master/software/spitest.cpp 
### Strace to figure out system calls
### g++ spitest.cpp -lwiringPi -lgpiod -o spitest && strace ./spitest 2>&1 | grep "(3," not needed
### during my testing file descriptor 3 was for GPIO and 4 was for SPI, it will change based on system used.

The speed is around 7.5 to 8 kHz, earlier tests seems to indicate it could do 16 kHz, i believe that this could be the fact that the wiringPiSPI uses a RW mode of SPI write, that means half the SPI cycles could be lost on reading. Which seems to be consistant with the 8kHz finding. Moving to a different chip will pose different concerns, but we will have to see how that works.

**** Packages Installed (May not be a complete list)****
sudo apt-get install spi-tools
sudo apt-get install pinout
sudo apt-get install gpio
sudo apt-get install wiringpi
  
***/
/******
##############     Wiring Information         #############
┌───────────────────────────────┬────────┬────────────────┐
│      Raspberry Pi 3 (B+)      │        │  EVAL-AD5370   │
├───┬───────┬──────────┬────────┤ Wire   ├────────┬───────┤
│ # │ Pin#  │ Type     │ Name   │ Colour │ Name   │ Pin#  │
├───┼───────┼──────────┼────────┼────────┼────────┼───────┤
│ 1 │ 11    │ GPIO     │ LDAC   │ RED    │ /LDAC  │ 8     │
│ 2 │ 13    │ GPIO     │ CLR    │ ORANGE │ /CLR   │ 10    │
│ 3 │ 15    │ GPIO     │ BUSY   │ YELLOW │ /BUSY  │ 12    │
│ 4 │ 7     │ GPIO     │ RESET  │ GREEN  │ /RESET │ 14    │
│ 5 │ 19    │ SPI/MOSI │ MOSI   │ BLUE   │ DIN    │ 1     │
│ 6 │ 21    │ SPI/MISO │ MISO   │ GREY   │ SDO    │ T6    │
│ 7 │ 23    │ SPI/SCLK │ SCLK   │ PURPLE │ SCLK   │ 3     │
│ 8 │ 34/39 │ Ground   │ GND    │ BLACK  │ DGND   │ 19/20 │
│ 9 │ 24    │ SPI/CE0  │ SS/CE0 │ BROWN  │ /SYNC  │ 6     │
└───┴───────┴──────────┴────────┴────────┴────────┴───────┘
** T6 is a test point for EVAL-AD5370
** Pin# for EVAL-AD5370 is from J3 Header
** Pin# for Raspberry Pi 3 B+ model is physical pin number
** The code uses WiringPi Pin numbering.
** Connect Atleast two grounds for stable connection

*******/
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
	fd = wiringPiSPISetupMode(CHANNEL, 500000,1);  //Returns the File Descriptor for SPI --- SPI mode 3, 2 ,1 works, SPI mode 0 DOES NOT WORK, I have not noticed any speed difference between 3 modes, need to read more about AD5370 and how this modes impact speed if it does or not.

	cout << "wiringPiSPISetupMode Init result: " << fd << endl;

	//Perform a Reset Activity
	digitalWrite(RESET_pin, LOW);
	usleep(1);
	digitalWrite(RESET_pin, HIGH);
	
	//Perform a Clear Activity
	digitalWrite(CLR_pin, LOW);
	usleep(1);
	digitalWrite(CLR_pin, HIGH);
	
	//Set LDAC_pin LOW for continious update write
	digitalWrite(LDAC_pin, LOW);
	cout << "Pre Loop"<< endl;
	for(int i=0;i<100000;i++)
	{
		//[201, 98, 32] (for +1 V) , 201, is channel 1, 200, is channel 0, 203, is channel 3
		buffer[0] = 201;
		buffer[1] = 98;
		buffer[2] = 32;
		if(digitalRead(BUSY_pin) == HIGH) //Ensuring the system (AD5370) is not busy so that we can write to the register, not important for one channel, but might be important for multi channel, if we remove this IF statement we get ~ 8kHz. See Prologue Note on SPEED
			wiringPiSPIDataRW(CHANNEL, buffer, 3); //The word has to be written simultaneously, the buffer is overwritten by the read activity (RW), after the write.

		//usleep(1); //Sleep to stabilize the loop, so far we have achieved 7.7 kHz, but it flickers around 7.5 to 7.8 kHz, to stabilize the loop, the usleep timer is used, may be more precision timers may be required for the future.
		
		//[201, 72, 135]   (for -1 V)		
		buffer[0] = 201;
		buffer[1] = 72;
		buffer[2] = 135;
		if(digitalRead(BUSY_pin) == HIGH)
			wiringPiSPIDataRW(CHANNEL, buffer, 3);
		
		//usleep(1);
	}
	cout << "Post Loop"<< endl;
}