#ifndef __SDS_H__
#define __SDS_H__

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <memory>
#include <sys/types.h>

#include "base.h" 

/*
 * 最大预分配长度
 */
#define SDS_MAX_PREALLOC (1024*1024)


typedef char *sds;
using std::allocator;
using std::ostream;
using std::cout;
using std::endl;
static allocator<char> alloc;

// static char *temp_buf = alloc.allocate(1024*1024); // 临时字符串缓冲区， 避免多余的系统调用


// 以下为辅助sds的工具函数

#define SDS_LLSTR_SIZE 21

int sdsll2str(char *s, long long value);
int sdsull2str(char *s, unsigned long long v);
int sdsmatch(void*, void*);

int is_hex_digit(char c);
int hex_digit_to_int(char c);



//sds类实现
class sdshdr: public BaseStruct{
    private:
        int len;
        int free;
        char *buf;

    public:

        sdshdr(const char *init, size_t initlen);
        sdshdr(const char *init);
        sdshdr(long long value);
        sdshdr();
        sdshdr(const sdshdr&);


        sdshdr& operator=(const sdshdr&);
        sdshdr& operator=(const char* t);
        sdshdr& operator=(long long value);
        sdshdr& operator+=(const sdshdr&);
        sdshdr& operator+=(const char *t);

        ~sdshdr(){
            alloc.deallocate(buf, len+free+1);
        }




        virtual size_t length() const {return len;};
        inline size_t sdslen() const {return len;};  // 返回sds实际保存字符串的长度
        inline size_t sdsavail() const {return free;}; // 返回sds剩余的可用长度
        inline char* get_buf() const {return buf;}; //返回字符串
        size_t sdsAllocSize() const {return sizeof(*this) + len + free + 1;};
        
        
        //惰性删除
        void sdsclear();

        /*
        * 对 sds 中 buf 的长度进行扩展，确保在函数执行之后，
        * buf 至少会有 addlen + 1 长度的空余空间
        * （额外的 1 字节是为 \0 准备的）
        *
        * 返回值
        *  sds ：扩展成功返回扩展后的 sds
        *        扩展失败抛bal_alloc
        *
        * 复杂度
        *  T = O(N)
        */

        // 可能抛bad_alloc
        sds sdsMakeRoomFor(size_t addlen);


        /*
        * 回收 sds 中的空闲空间，
        * 回收不会对 sds 中保存的字符串内容做任何修改。
        *
        * 返回值
        *  sds ：内存调整后的 sds
        *
        * 复杂度
        *  T = O(N)
        */
        sds sdsRemoveFreeSpace();


        // * 根据 incr 参数，增加 sds 的长度，缩减空余空间，
        // * 并将 \0 放到新字符串的尾端
        // *  * 这个函数是在调用 sdsMakeRoomFor() 对字符串进行扩展，
        // * 然后用户在字符串尾部写入了某些内容之后，
        // * 用来正确更新 free 和 len 属性的。
        // *  * 如果 incr 参数为负数，那么对字符串进行右截断操作。
        void sdsIncrLen(int incr);


        //扩充sds至指定长度， 未使用空间填0
        //成功返回sds 失败抛bad_alloc
        sds sdsgrowzero(size_t len_);


        // 将长度为len的字符串追加到sds末尾
        // 成功返回新sds， 失败抛bad_alloc
        // // 因使用了临时缓冲区 线程不安全 多线程环境下需上锁
        // 放弃全局buffer， 改为根据大小优先从栈上分配
        sds sdscatlen(const void *t, size_t addlen);


        //将t的前addlen个字符复制到sds中， 并在结尾加'/0‘
        //如果len少于addlen， 扩展
        sds sdscpylen(const char* t, size_t addlen);


        /*
        * 对 sds 左右两端进行修剪，清除其中 cset 指定的所有字符
        *
        * 比如 sdsstrim(xxyyabcyyxy, "xy") 将返回 "abc"
        *
        * 复杂性：
        *  T = O(M*N)，M 为 SDS 长度， N 为 cset 长度。
        */
        sds sdstrim(const char *cset);

        /*
        * 按索引对截取 sds 字符串的其中一段
        * start 和 end 都是闭区间（包含在内）
        *
        * 索引从 0 开始，最大为 sdslen(s) - 1
        * 索引可以是负数， sdslen(s) - 1 == -1
        *
        * 复杂度
        *  T = O(N)
        */
        void sdsrange(int start, int end);
        
        void sdstolower() const;
        void sdstoupper() const;


        /*
        * 对比另一个 sds ， strcmp 的 sds 版本
        *
        * 返回值
        *  int ：相等返回 0 ，自己较大返回正数， 另一个较大返回负数
        *
        * T = O(N)
        */
        int sdscmp(const sdshdr& s2) const;


        sds sdsmapchars(const char *form, const char *to, size_t setlen);
        //调试环境下的函数
        void sds_info(){
             cout << get_buf() << ' ' << sdslen() << ' '<< sdsavail() << endl;
        }

};


// sdshdr operator+(const sdshdr& lhs, const sdshdr& rhs);
// sdshdr operator+(const sdshdr& lhs, const char* rhs);

template <typename T> sdshdr* testadd(sdshdr const *, const T &);
template <typename T> sdshdr* testadd(const sdshdr *lhs, const T &rhs){
    cout << "testadd: " << lhs->get_buf() << endl;

    sdshdr *sum = new sdshdr(*lhs);
    cout << "testadd: " << sum->get_buf() << " " << sum->sdslen() << " " <<sum->sdsavail() << endl;

    *sum += *rhs;
    cout << "testadd: " << sum->get_buf() << " " << sum->sdslen() << " " <<sum->sdsavail() << endl;
    return sum;
}

// template <typename T> sdshdr* operator+(const sdshdr &lhs, const T &rhs);
// template <typename T> sdshdr* operator+(const sdshdr &lhs, const T &rhs){
//     cout << "testadd: " << lhs->get_buf() << endl;

//     sdshdr *sum = new sdshdr(*lhs);
//     cout << "testadd: " << sum->get_buf() << " " << sum->sdslen() << " " <<sum->sdsavail() << endl;

//     *sum += *rhs;
//     cout << "testadd: " << sum->get_buf() << " " << sum->sdslen() << " " <<sum->sdsavail() << endl;
//     return sum;
// }
sdshdr* operator+(const sdshdr& lhs, const sdshdr& rhs);
sdshdr* operator+(const sdshdr& lhs, const char* t);

bool operator==(const sdshdr& lhs, const sdshdr& rhs);
ostream& operator<<(ostream&, const sdshdr&);

sdshdr* sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);
/*
 * 使用分隔符 sep 对 s 进行分割，返回一个 sds 字符串的数组。
 * count 会被设置为返回数组元素的数量。
 * 如果出现字符串长度为 0 或分隔符长度为 0
 * 的情况，返回 NULL
 * 注意分隔符可以的是包含多个字符的字符串
 *

 * 这个函数接受 len 参数，因此它是二进制安全的。
 * （文档中提到的 sdssplit() 已废弃）
*/

sdshdr* sdsjoin(char **argv, int argc, char *sep);
// template <typename T> sdshdr* operator+(const sdshdr *lhs, const T &rhs);
// template <typename T> sdshdr* operator+(const sdshdr *lhs, const T &rhs){
//     cout << "testadd: " << lhs->get_buf() << endl;

//     sdshdr *sum = new sdshdr(*lhs);
//     cout << "testadd: " << sum->get_buf() << " " << sum->sdslen() << " " <<sum->sdsavail() << endl;

//     *sum += *rhs;
//     cout << "testadd: " << sum->get_buf() << " " << sum->sdslen() << " " <<sum->sdsavail() << endl;
//     return sum;
// }



#endif