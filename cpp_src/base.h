#include "sys/types.h"

class BaseStruct{
    public:
        BaseStruct() = default;
        virtual ~BaseStruct() = default;

        virtual size_t length() const = 0;


};

