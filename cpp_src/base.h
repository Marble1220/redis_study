#include "sys/types.h"

class base_struct{
    public:
        base_struct() = default;
        virtual ~base_struct() = default;

        virtual size_t length() const = 0;


};


