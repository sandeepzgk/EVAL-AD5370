#include "DAC.h"

namespace AD537x
{
	/**
	* Desc: Class Constructor of singleton DAC class, that loads the DLL library
	*		and loads the function pointers from the DLL and maps it to the typedefs in
	*		the header files.
	* Param: None
	*/
	DAC::DAC()
	{
		hinstDLL = LoadLibrary(DLL_PATH);
		if (hinstDLL == 0)
		{
			std::cout << "ERROR: DLL was not initialized." << std::endl;
		}

		Search_For_Boards = (SearchFunction)GetProcAddress(hinstDLL, "Search_For_Boards");
		Connect = (ConnectFunction)GetProcAddress(hinstDLL, "Connect");
		Download_Firmware = (DownloadFWFunction)GetProcAddress(hinstDLL, "Download_Firmware");
		Vendor_Request = (VendorRequestFunction)GetProcAddress(hinstDLL, "Vendor_Request");
		Disconnect = (DisconnectFunction)GetProcAddress(hinstDLL, "Disconnect");
		if (!Search_For_Boards || !Connect || !Vendor_Request || !Disconnect || !Download_Firmware)
		{
			std::cout << "ERROR: One of the DLL function is empty." << std::endl;
			FreeLibrary(hinstDLL);
		}

	}

	/**
	* Desc: Class Destructor of singleton DAC class, that frees the DLL loaded in constructor
	* Param: None
	*/
	DAC::~DAC() //Destructor
	{
		FreeLibrary(hinstDLL);
	}

	/**
	* Desc: Discovers all the connected EVAL-AD5370 boards via USB,
	*		Attempts to connect, download firware and intialize them for use
	* Param: None
	*/
	int DAC::find_and_initialize_all_boards()
	{
		int searchResult = search_for_boards();
		for (int i = 0; i < _numBoards; i++)
		{
			int connectResult = connect_board(i);
			int FWResult = download_firmware(i);
			int initResult = initialize_vendor_request(i);

			if (connectResult != 0)
				std::cout << "ERROR: "<< connectResult << " -Connection Issue for Board: " << i << std::endl;
			if (FWResult != 0)
				std::cout << "ERROR: " << FWResult << " -Firmware Upload Issue for Board: " << i << std::endl;
			if (initResult != 0)
				std::cout << "ERROR: " << initResult << " -Initialize Issue for Board: " << i << std::endl;

			return connectResult + FWResult +initResult;
		}
	}

	/**
	* Desc: Search for boards (upto MAX_BOARDS) connected to the system
	*		It loops through each device and populates the "devices" vector storing the 
	*		path returned from the search function
	* Param: None
	*/
	int DAC::search_for_boards()
	{

		unsigned char devicePaths[MAX_BOARDS];
		int retValue = Search_For_Boards(_VENDOR_ID, _PRODUCT_ID, &_numBoards, devicePaths);
		for (int i = 0; i < _numBoards; i++)
		{
			DeviceHandles device;
			device.path = devicePaths[i];
			devices.push_back(device);
		}
		return retValue;
	}

	/**
	* Desc: Downloads the firmware present in the FW_PATH to the device_index board
	* Param: device_index - the index of the board in the devices vector
	*/
	int DAC::download_firmware(int device_index)
	{
		return Download_Firmware(devices[device_index].handle, FW_PATH);
	}

	/**
	* Desc: Sends the initialize vendor request to the device to initialize further vendor requests
	* Param: device_index - the index of the board in the devices vector
	*/
	int DAC::initialize_vendor_request(int device_index)
	{
		return Vendor_Request(devices[device_index].handle,
			VR_INIT, 0, 0, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

	/**
	* Desc: Takes a hexadecimal word and splits to different value and index parameters.
	* Param: device_index - the index of the board in the devices vector
	*		 word         - the hexadecimal word that needs to be written to the board for speical commands
	*/
	int DAC::write_spi_word(int device_index, std::string word)
	{
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

	/**
	* Desc: Takes a hexadecimal word and splits to different value and index parameters.
	* Param: device_index - the index of the board in the devices vector
	*/
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

}  // namespace AD537x
