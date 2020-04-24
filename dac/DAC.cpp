#include "DAC.h"

namespace AD537x
{
	DAC::DAC() //Constructor
	{
		std::cout << "Loading DLL: \"" << DLL_PATH << "\"" << std::endl;
		hinstDLL = LoadLibrary(DLL_PATH);
		if (hinstDLL == 0)
		{
			std::cout << "ERROR: hinstDLL is NULL!" << std::endl;
		}

		Search_For_Boards = (SearchFunction)GetProcAddress(hinstDLL, "Search_For_Boards");
		Connect = (ConnectFunction)GetProcAddress(hinstDLL, "Connect");
		Download_Firmware = (DownloadFWFunction)GetProcAddress(hinstDLL, "Download_Firmware");
		Vendor_Request = (VendorRequestFunction)GetProcAddress(hinstDLL, "Vendor_Request");
		Disconnect = (DisconnectFunction)GetProcAddress(hinstDLL, "Disconnect");
		if (!Search_For_Boards || !Connect || !Vendor_Request || !Disconnect || !Download_Firmware)
		{
			std::cout << "ERROR: One of the DLL Processes returned NULL!" << std::endl;
			FreeLibrary(hinstDLL);
		}

	}

	DAC::~DAC() //Destructor
	{
		FreeLibrary(hinstDLL);
	}


	int DAC::write_spi_word(int device_index, std::string word)
	{
		/****
			Write SPI word by calling the Vendor_Request DLL call. Takes a hexadecimal word
			and splits to different value and index parameters.
			Function prototype : Int Vendor_Request(UInt  Handle, UChar Request, UShort Value, UShort Index,
													UChar Direction, UShort DataLength, UChar * Buffer[]);
		****/

		std::stringstream index_string, value_string;
		unsigned short index;
		unsigned short value;

		index_string << std::hex << word.substr(0, 2);
		value_string << std::hex << word.substr(2, 6);

		index_string >> index;
		value_string >> value;

		return Vendor_Request(devices[device_index].handle,
			VR_SPI_WRITE, value, index, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

	int DAC::connect_board(int device_index)
	{
		return Connect(_VENDOR_ID, _PRODUCT_ID, devices[device_index].path, &devices[device_index].handle);
	}


	int DAC::write_voltage(int device_index, int channel, float voltage_target)
	{
		unsigned short value = round(((voltage_target + _vOffset) / _vRange) * 65535);
		return Vendor_Request(devices[device_index].handle,
			VR_SPI_WRITE, value, 0xC8+channel, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

	int DAC::pulse_ldac(int device_index)
	{
		return Vendor_Request(devices[device_index].handle,
			VR_PULSE_LDAC, VR_ZERO, VR_ZERO, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

	int DAC::download_firmware(int device_index)
	{
		std::cout << "Downloading Firmware: \"" << FW_PATH << "\"" << std::endl;
		int dwVal = Download_Firmware(devices[device_index].handle, FW_PATH);
		int initVal = initialize_vendor_request(device_index);

		if (initVal == 0 && dwVal == 0)
			return 0;
		else
			return 1;

	}

	int DAC::initialize_vendor_request(int device_index)
	{
		return Vendor_Request(devices[device_index].handle,
			VR_INIT, 0, 0, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}


	int DAC::search_for_boards()
	{
		
		unsigned char devicePaths[MAX_BOARDS];
		int retValue = Search_For_Boards(_VENDOR_ID, _PRODUCT_ID, &_numBoards,devicePaths);
		for (int i = 0; i < _numBoards; i++)
		{
			DeviceHandles device;
			device.path = devicePaths[i];
			devices.push_back(device);
		}		
		return retValue;
	}

}  // namespace AD537x
