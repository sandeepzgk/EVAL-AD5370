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


 // Defines
#define VENDOR_ID 1110    //HEX VALUE - 0x0456 
#define PRODUCT_ID 45576  //HEX VALUE - 0xB208 

// Vendor Request Commands
#define VR_REQUEST 0xDE
#define VR_VALUE 0xD
#define DIR_READ 0x01
#define DIR_WRITE 0x00


namespace AD537x 
{
    
    
    class DAC : public Singleton<DAC>
    {
        template <typename DAC>
        friend class Singleton;

        typedef int (CALLBACK* SearchFunction) (int, int, int *, unsigned char* const);
        typedef int (CALLBACK* ConnectFunction) (unsigned int, unsigned int, char, unsigned int*);
        typedef int (CALLBACK* DownloadFWFunction) (unsigned int, char*);
        typedef int (CALLBACK* VendorRequestFunction) (unsigned int, char, unsigned short, unsigned short, char, unsigned short, char*);
        typedef int (CALLBACK* DisconnectFunction) (unsigned int);


        private:
            long _VID;
            long _PID;
            char* _handle;
            unsigned int _numBoards = 0;
            unsigned char* _PartPath;

            // Load Functions From DLL
            SearchFunction Search_For_Boards;
            ConnectFunction Connect;
            DownloadFWFunction Download_Firmware;
            VendorRequestFunction Vendor_Request;
            DisconnectFunction Disconnect;

            HMODULE hinstDLL;
           
        protected:
            //List of protected variables and functions

        public:
            DAC(long vid = 1110, long pid = 45583);
            ~DAC();
           

            int write_spi_word(std::string word);

            int connect_board();

            int search_for_boards();

          

    };
}  // namespace AD537x
#endif  // DAC_H
