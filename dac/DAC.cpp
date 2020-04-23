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
            std::cout << "ERROR: One of the DLL Processes returned NULL!"<<std::endl;
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

        //std::cout << "word: " << word << std::endl;
        //std::cout << "value_string EXPT: " << value_string.str() << std::endl;
        //std::cout << "request: " << request << std::endl;
        //std::cout << "index: " << index << std::endl;
        //std::cout << "value: " << value << std::endl;
        //std::cout << "zero: " << zero << std::endl;
        //std::cout << "direction: " << direction << std::endl;
        //std::cout << "buffer: " << buffer << std::endl;
        //std::cout << "##############" << std::endl;
        int retVal = Vendor_Request(devices[device_index].handle, 
            VR_REQUEST_SPI_WRITE,value, index, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
        return retVal;
    }
    
    int DAC::connect_board(int device_index)
    {
        int retVal =  Connect(_VENDOR_ID, _PRODUCT_ID, devices[device_index].path[0], &devices[device_index].handle);         
        //devices[device_index].handle = 4;
        return retVal;

    }

    std::string DAC::channel_to_hex(int channel_number)
    {
        std::div_t dv{};
        dv = std::div(channel_number, 8);
        unsigned long group_no  = dv.quot + 1;
        std::string group       = std::bitset<3>(group_no).to_string();
        std::string channel     = std::bitset<3>(dv.rem).to_string(); 
        std::string reg         = "11"; //X Register
        std::string word        = reg + group + channel;
        unsigned long value     = std::bitset<8>(word).to_ulong();
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
        int retVal = DAC::write_spi_word(device_index, word);
        return retVal;
    }
    
    int DAC::pulse_ldac(int device_index)
    {        
        int retVal = Vendor_Request(devices[device_index].handle, 
            VR_REQUEST_PULSE_LDAC, VR_ZERO, VR_ZERO, VR_DIR_WRITE, VR_ZERO_DATA_LENGTH, &_emptyBuffer);
        return retVal;
    }

    int DAC::download_firmware(int device_index)
    {
        std::cout << "Downloading Firmware: \"" << FW_PATH << "\"" << std::endl;
        int retVal = Download_Firmware(devices[device_index].handle, FW_PATH);
        
        return retVal;
    }
    
    int DAC::search_for_boards()
    {
        DeviceHandles device;
        int retValue =  Search_For_Boards(_VENDOR_ID, _PRODUCT_ID, &_numBoards, device.path);
        devices.push_back(device);
        return retValue;
    }

}  // namespace AD537x
