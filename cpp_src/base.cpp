#include "base.h"


unsigned int getLRUClock(void){
    return (timeInMilliseconds()/LRU_CLOCK_RESOLUTION) & LRU_CLOCK_MAX;
}