
#ifndef __BASE_H__
#define __BASE_H__

#include <sys/types.h>
#include <ctime>
#include <sys/time.h>

#define SDSSTRUCT  101
#define INTSETSTRUCT 102
#define ADLISTSTRUCT 103
#define SKIPLISTSTRUCT 104
#define DICTSTRUCT 105
#define INTSTRUCT 106

// lru相关
// lru精度
#define LRU_CLOCK_RESOLUTION  1000
// 最大时间
#define LRU_CLOCK_MAX ((1<<24)-1)



// 用于python端和cpp端通信 
#define PY_ERR 0
#define PY_OK 1

// 用于在控制链表结构的插入和删除的头尾
#define HEAD 0
#define TAIL 1


//shared对象相关
#define SHARED_INTEGERS 10000

// 记录内存使用的全局变量


class BaseStruct{
    public:
        int struct_type;
        BaseStruct(int type): struct_type(type) {};
        virtual ~BaseStruct() = default;

        virtual size_t length() const = 0;
};


typedef struct{
    double min, max;
    int minex, maxex;   //开闭标示
}rangespec;

//  返回以毫秒为单位的 UNIX 时间戳
static long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

unsigned int getLRUClock(void);


#endif