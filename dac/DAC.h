/*
 *
 */

#ifndef DAC_H
#define DAC_H

#include "../Utilities/Singleton.h"

#include <array>
#include <map>


namespace AD537x 
{

    class DAC : public Singleton<DAC>
    {
        template <typename DAC>
        friend class Singleton;
        private:
            //List of private variables and functions
        protected:
            //List of protected variables and functions

        public:
            DAC();
            ~DAC();

    };
}  // namespace AD537x
#endif  // DAC_H
