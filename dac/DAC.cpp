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
		return Connect(_VENDOR_ID, _PRODUCT_ID, devices[device_index].path[0], &devices[device_index].handle);
	}

	std::string DAC::channel_to_hex(int channel_number)
	{
		std::div_t dv{};
		dv = std::div(channel_number, 8);
		unsigned long group_no = dv.quot + 1;
		std::string group = std::bitset<3>(group_no).to_string();
		std::string channel = std::bitset<3>(dv.rem).to_string();
		std::string reg = "11"; //X Register
		std::string word = reg + group + channel;
		unsigned long value = std::bitset<8>(word).to_ulong();
		std::stringstream stream;
		stream << std::hex << value;
		return stream.str();
	}

	std::string DAC::voltage_to_hex(float voltage_target, float v_max = 10.f, float v_min = -10.f)
	{
		std::stringstream stream;

		float offset = (v_max - v_min) / 2.f;
		float value = ((voltage_target + offset) / (v_max - v_min)) * 65535;

		stream << std::hex << (unsigned long)(round(value));

		return stream.str();
	}

	int DAC::write_voltage(int device_index, int channel, float voltage)
	{
		std::string index = channel_to_hex(channel);
		std::string value = voltage_to_hex(voltage);

		std::cout << "Index: " << index << std::endl;
		std::cout << "value: " << value << std::endl;
		std::string word = index + value;
		return DAC::write_spi_word(device_index, word);
	}

	int DAC::pulse_ldac(int device_index)
	{
		return Vendor_Request(devices[device_index].handle,
			VR_PULSE_LDAC, VR_ZERO, VR_ZERO, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
	}

	int DAC::download_firmware(int device_index)
	{
		char a[14] = { 65, 68, 53, 51, 55, 120, 83, 80, 73, 46, 104, 101, 120, 0 };

		std::cout << "Downloading Firmware: \"" << a[0] << "\"" << std::endl;
		int dwVal = Download_Firmware(devices[device_index].handle, a);
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
		DeviceHandles device;
		int retValue = Search_For_Boards(_VENDOR_ID, _PRODUCT_ID, &_numBoards, device.path);
		devices.push_back(device);
		return retValue;
	}

}  // namespace AD537x
