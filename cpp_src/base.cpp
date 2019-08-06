#include <limits.h>
#include "base.h"


unsigned int getLRUClock(void){
    return (timeInMilliseconds()/LRU_CLOCK_RESOLUTION) & LRU_CLOCK_MAX;
}

// 尝试将长度为slen的字符串s转化为long long 
int string2ll(const char *s, size_t slen, long long *value){
    const char *p = s;
    size_t plen = 0;
    int negative = 0;
    unsigned long long v;

    if (plen == slen) return 0;
    if (slen == 1 && p[0] == '0'){
        if (value != nullptr) *value = 0;
        return 1;
    }

    if (p[0] == '-'){
        negative = 1;
        p++; plen++;
        if (plen == slen) return 0;
    }

    if (p[0] >= '1' && p[0] <= '9') {
        v = p[0]-'0';
        p++; plen++;
    } else if (p[0] == '0' && slen == 1) {
        *value = 0;
        return 1;
    } else {
        return 0;
    }

    while (plen < slen && p[0] >= '0' && p[0] <= '9') {
        if (v > (ULLONG_MAX / 10)) /* Overflow. */
            return 0;
        v *= 10;

        if (v > (ULLONG_MAX - (p[0]-'0'))) /* Overflow. */
            return 0;
        v += p[0]-'0';

        p++; plen++;
    }

        if (plen < slen)
        return 0;

    if (negative) {
        if (v > ((unsigned long long)(-(LLONG_MIN+1))+1)) /* Overflow. */
            return 0;
        if (value != NULL) *value = -v;
    } else {
        if (v > LLONG_MAX) /* Overflow. */
            return 0;
        if (value != NULL) *value = v;
    }
    return 1;

}