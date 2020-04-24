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
			std::cout << "ERROR: DLL was not initialized." << std::endl;
		
		Bulk_Transfer = (BulkTransferFunction)GetProcAddress(hinstDLL, "Bulk_Transfer");
		Connect = (ConnectFunction)GetProcAddress(hinstDLL, "Connect");
		Disconnect = (DisconnectFunction)GetProcAddress(hinstDLL, "Disconnect");
		Download_Firmware = (DownloadFWFunction)GetProcAddress(hinstDLL, "Download_Firmware");
		Get_String_Descriptor = (GetStringDescriptorFunction)GetProcAddress(hinstDLL, "Get_String_Descriptor");
		Retrieve_EndPoint_Details = (RetriveEndpointDetailsFunction)GetProcAddress(hinstDLL, "Retrieve_EndPoint_Details");
		Search_For_Boards = (SearchFunction)GetProcAddress(hinstDLL, "Search_For_Boards");
		Vendor_Request = (VendorRequestFunction)GetProcAddress(hinstDLL, "Vendor_Request");

		if (!Search_For_Boards || !Connect || !Vendor_Request || !Disconnect || !Download_Firmware ||
			!Get_String_Descriptor || !Retrieve_EndPoint_Details || Bulk_Transfer)
		{
			std::cout << "ERROR: One of the DLL function is empty." << std::endl;
			FreeLibrary(hinstDLL);
		}
	}

	/**
	* Desc: Class Destructor of singleton DAC class, that frees the DLL loaded in constructor
	* Param: None
	*/
	DAC::~DAC()
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
	* Desc: Sends the initialize vendor request to the device to support future VR requests
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
	*		 word         - the hexadecimal word that needs to be written to the board for 
							speical commands
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
	* Desc: Connects to a particular device, and populates the handle in the devices vector for 
	*		that particular path
	* Param: device_index - the index of the board in the devices vector
	*/
	int DAC::connect_board(int device_index)
	{
		return Connect(_VENDOR_ID, _PRODUCT_ID, devices[device_index].path, &devices[device_index].handle);
	}

	/**
	* Desc: Creates and issues a vendor request (VR) that sets a particular voltage at a 
	*		particular channel for a particular device.
	*		Note 1: the channel computation uses a complex method based on channel, 
	*				and channel group this is documented in the older version of code in 
	*				function channel_to_hex in https://bit.ly/354O4UF
	*				This function boils down to "0xC8 + channel", which is used here to
	*				speed up required time
	*		Note 2: the voltage computation takes a target value in the _vRange, and maps it 
	*				to 0-65535, i.e. the range of unsigned short.
	* Param: device_index	- the index of the board in the devices vector
	*		 channel		- provides the enumeration of channel where for the voltage
	*						  i.e. 0 to 39 for EVAL-AD5370
	*		 voltage_target - the target voltage the system needs to write to the board
	*						  i.e usually within a range of _vMin to _vMax
	*/
	int DAC::write_voltage(int device_index, int channel, float voltage_target)
	{
		unsigned short value = round(((voltage_target + _vOffset) / _vRange) * 65535);
		return Vendor_Request(devices[device_index].handle,
			VR_SPI_WRITE, value, 0xC8+channel, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

	/**
	* Desc: Creates the LDAC Pulse signal for updating the DAC to present new register
	*		values
	* Param: device_index - the index of the board in the devices vector
	*/
	int DAC::pulse_ldac(int device_index)
	{
		return Vendor_Request(devices[device_index].handle,
			VR_PULSE_LDAC, VR_ZERO, VR_ZERO, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

}  // namespace AD537x
