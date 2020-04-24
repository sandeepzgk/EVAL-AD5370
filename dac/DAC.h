/*
 *
 */

#ifndef DAC_H
#define DAC_H

#include "../Utilities/Singleton.h"

#include <iostream>
#include <Windows.h>
#include <vector>
#include <sstream>

// Vendor Request Commands
#define VR_SPI_WRITE 0xDD
#define VR_SPI_READ 0xDD
#define VR_SETRESET 0xDA
#define VR_CLRRESET 0xDB
#define VR_SETCLR 0xDC
#define VR_PULSE_LDAC 0xDE
#define VR_CLRCLR 0xDF
#define VR_SETLDAC 0xE2
#define VR_CLRLDAC 0xE3
#define VR_DIR_READ 1
#define VR_DIR_WRITE 0
#define VR_ZERO_DATA_LENGTH 0
#define VR_ZERO 0
#define VR_INIT 0xE0
#define VR_PORTOUT 0xE1

// Application Specific Constants 
#define MAX_BOARDS 100
#define MAX_CHANNELS 40
#define DLL_PATH TEXT("C:\\Program Files\\Analog Devices\\USB Drivers\\ADI_CYUSB_USB4.dll")	//Default driver installation path on windows. and default path to the DLL


namespace AD537x
{
	//Structure to hold information of path and handles of multiple devices
	struct DeviceHandles {
		unsigned char path;
		int handle = -1;
	};

	class DAC : public Singleton<DAC>
	{
		template <typename DAC>
		friend class Singleton;

		//DLL function prototypes
		typedef int (CALLBACK* BulkTransferFunction) (int, unsigned char, unsigned int&, unsigned char* const);
		typedef int (CALLBACK* ConnectFunction) (int, int, unsigned char, int*);
		typedef int (CALLBACK* DisconnectFunction) (unsigned int);
		typedef int (CALLBACK* DownloadFWFunction) (int, char[]);		
		typedef int (CALLBACK* GetStringDescriptorFunction) (int, int*, unsigned char*, unsigned char);
		typedef int (CALLBACK* RetriveEndpointDetailsFunction) (int, unsigned char*, char*);
		typedef int (CALLBACK* SearchFunction) (int, int, int*, unsigned char*);
		typedef int (CALLBACK* VendorRequestFunction) (int, unsigned char, unsigned short, unsigned short, unsigned char, unsigned short, unsigned char*);

	private:
		//Vendor ID and Product ID for AD5370 (evaluation board)
		const int _VENDOR_ID = 1110;						//HEX VALUE - 0x0456 
		const int _PRODUCT_ID = 45583;						//HEX VALUE - 0xB208 

		//@DLR #TODO
		//Fully Qualified Path to Firmware ending with a \0. 
		//This cannot be a #define because, the function requires char * not char_t?? 
		//Partial paths seems to cause issues., can it be made into a parameter for class construction? 
		char FW_PATH[100] = "C:\\code\\AD537x\\Binaries\\AD537xSPI.hex\0";
		
		unsigned char _emptyBuffer;							//Empty Buffer to send while calling write Vendor Requests

		int _numBoards = 0;									//Number of Boards attached to the system via USB
		
		const float _vMax = +6.f;							//Max Voltage of the board based on Jumper Setting
		const float _vMin = -6.f;							//Min Voltage of the board based on Jumper Setting
		const float _vRange = _vMax - _vMin;				//Voltage Range of the board, used for computing voltage values to be sent via SPI
		const float _vOffset = _vRange / 2.f;				//Offset Voltage of the board, used for computing voltage values to be sentvia SPI

		HMODULE hinstDLL;									//HModule to load and store DLL symbols

		//Instances of the typedef of the DLL function prototypes		
		BulkTransferFunction Bulk_Transfer;
		ConnectFunction Connect;
		DisconnectFunction Disconnect;
		DownloadFWFunction Download_Firmware;
		GetStringDescriptorFunction Get_String_Descriptor;
		RetriveEndpointDetailsFunction Retrieve_EndPoint_Details;
		SearchFunction Search_For_Boards;
		VendorRequestFunction Vendor_Request;

		int initializeVendorRequest(int device_index);
		int downloadFirmware(int device_index);
		int searchForBoards();
		int connectBoard(int device_index);
		int disconnectBoard(int device_index);
		int initalizeAllChannels(int device_index, int max_channels = MAX_CHANNELS);


	public:
		std::vector<DeviceHandles> devices;
		DAC();
		~DAC();
		
		int writeSPIWord(int device_index, std::string word);		
		int writeVoltage(int device_index, int channel, float voltage);
		int pulseLDAC(int device_index);
		int findAndInitializeAllBoards();
	};
}  // namespace AD537x
#endif  // DAC_H
