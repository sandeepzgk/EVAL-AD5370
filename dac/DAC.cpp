#include "DAC.h"

namespace AD537x 
{
    DAC::DAC() //Constructor
    {

        hinstDLL = LoadLibrary(TEXT("C:\\Program Files\\Analog Devices\\USB Drivers\\ADI_CYUSB_USB4.dll"));
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
  

    int DAC::write_spi_word(std::string word)
    {
        /**** 
            Write SPI word by calling the Vendor_Request DLL call. Takes a hexadecimal word
            and splits to different value and index parameters.
            Function prototype : Int Vendor_Request(UInt  Handle, UChar Request, UShort Value, UShort Index,
                                                    UChar Direction, UShort DataLength, UChar * Buffer[]);
        ****/
        unsigned int request;
        unsigned short index;
        unsigned short value;
        short zero = 0;
        unsigned char direction = 0;
        char* buffer;
        
       // return Vendor_Request(_handle, request,value, index, direction, zero, &buffer);
        return 0;
    }
    
    int DAC::connect_board(int device_index)
    {
        return Connect(_VENDOR_ID, _PRODUCT_ID, devices[device_index].path[0], &devices[device_index].handle);         
    }
    

    int DAC::download_firmware()
    {
        return 0;
    }
    
    int DAC::search_for_boards()
    {
        DeviceHandles device;
        int retValue =  Search_For_Boards(_VENDOR_ID, _PRODUCT_ID, &_numBoards, device.path);
        devices.push_back(device);
        return retValue;
    }

}  // namespace AD537x
