/*
 *
 */

#ifndef DAC_H
#define DAC_H

#include "../Utilities/Singleton.h"

#include <array>
#include <map>
#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <bitset>

 // Defines


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


#define VR_VALUE 0xD

#define DLL_PATH TEXT("C:\\Program Files\\Analog Devices\\USB Drivers\\ADI_CYUSB_USB4.dll")


namespace AD537x
{
	struct DeviceHandles {
		unsigned char path[1];
		int handle = -1;
	};

	class DAC : public Singleton<DAC>
	{
		template <typename DAC>
		friend class Singleton;

		typedef int (CALLBACK* SearchFunction) (int, int, int*, unsigned char* const);
		typedef int (CALLBACK* ConnectFunction) (int, int, unsigned char, int*);
		typedef int (CALLBACK* DownloadFWFunction) (int, char[]);
		typedef int (CALLBACK* VendorRequestFunction) (int, unsigned char, unsigned short, unsigned short, unsigned char, unsigned short, unsigned char*);
		typedef int (CALLBACK* DisconnectFunction) (unsigned int);


	private:
		const int _VENDOR_ID = 1110;   //HEX VALUE - 0x0456 
		const int _PRODUCT_ID = 45583;  //HEX VALUE - 0xB208 

		char FW_PATH[100] = "C:\\code\\AD537x\\Binaries\\AD537xSPI.hex0\0";
		unsigned char _emptyBuffer;

		int _numBoards = 0;


		// Load Functions From DLL
		SearchFunction Search_For_Boards;
		ConnectFunction Connect;
		DownloadFWFunction Download_Firmware;
		VendorRequestFunction Vendor_Request;
		DisconnectFunction Disconnect;

		HMODULE hinstDLL;


		std::string channel_to_hex(int channel);
		std::string voltage_to_hex(float voltage_target, float v_max, float v_min);
		int initialize_vendor_request(int device_index);
	protected:
		//List of protected variables and functions

	public:
		std::vector<DeviceHandles> devices;
		DAC();
		~DAC();

		int write_spi_word(int device_index, std::string word);
		int connect_board(int device_index);
		int write_voltage(int device_index, int channel, float voltage);
		int pulse_ldac(int device_index);
		int download_firmware(int device_index);

		int search_for_boards();
	};
}  // namespace AD537x
#endif  // DAC_H
