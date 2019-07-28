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
        
        
        
        void sdsclear() {
            //惰性删除
            free += len;
            len = 0;
            buf[0] = '\0';
        }

        sds sdsMakeRoomFor(size_t addlen) {    // 可能抛bad_alloc
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
            size_t newlen;

            if (free > addlen) {

                return buf;
            }
            newlen = len + addlen;

            if (newlen < SDS_MAX_PREALLOC)
                newlen *= 2;
            else
                newlen += SDS_MAX_PREALLOC;
            

            auto newbuf = alloc.allocate(newlen + 1);
            memcpy(newbuf, buf, len);
            newbuf[len] = '\0';
            alloc.deallocate(buf, len+free+1);
            
            free = newlen - len;
            buf = newbuf;

            return buf;

        }

        sds sdsRemoveFreeSpace() {
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
            if (free == 0) return buf;
            auto newbuf = alloc.allocate(len+1);
            memcpy(newbuf, buf, len+1);
            alloc.deallocate(buf, len+1);
            buf = newbuf;
            free = 0;
            return buf;
        }


        void sdsIncrLen(int incr){
        // * 根据 incr 参数，增加 sds 的长度，缩减空余空间，
        // * 并将 \0 放到新字符串的尾端
        // *  * 这个函数是在调用 sdsMakeRoomFor() 对字符串进行扩展，
        // * 然后用户在字符串尾部写入了某些内容之后，
        // * 用来正确更新 free 和 len 属性的。
        // *  * 如果 incr 参数为负数，那么对字符串进行右截断操作。
        assert(free >= incr);

        len += incr;
        free -= incr;

        buf[len] = '\0';
        }

        sds sdsgrowzero(size_t len_){
            //扩充sds至指定长度， 未使用空间填0
            //成功返回sds 失败抛bad_alloc

            size_t totlen, curlen = len;

            if (len_ <= curlen) return buf;

            buf = sdsMakeRoomFor(len_ - curlen);

            std::uninitialized_fill_n(buf+curlen, (len_ - curlen), 0);
            buf[len_] = '\0';
            totlen = len + free;
            len = len_;
            free = totlen - len;

            return buf;
        }

        sds sdscatlen(const void *t, size_t addlen){
            // 将长度为len的字符串追加到sds末尾
            // 成功返回新sds， 失败抛bad_alloc
            // // 因使用了临时缓冲区 线程不安全 多线程环境下需上锁
            // 放弃全局buffer， 改为根据大小优先从栈上分配
            size_t curlen = len;
            char *temp_buf;
            char temp_[64];
            bool flag = false;
            if (addlen < 64)
                temp_buf = temp_;
            else{
                temp_buf = alloc.allocate(addlen+1);
                flag = true;
            }

            memcpy(temp_buf, t, addlen);
            
            auto s = sdsMakeRoomFor(addlen);

            memcpy(s+curlen, temp_buf, addlen);

            len = len + addlen;
            free -= addlen;
            s[len] = '\0';

            if (flag)
                alloc.deallocate(temp_buf, addlen+1);
            return s;
        }

        sds sdscpylen(const char* t, size_t addlen){
            //将t的前addlen个字符复制到sds中， 并在结尾加'/0‘
            //如果len少于addlen， 扩展

            size_t totlen = len + free;
            if (totlen < addlen){
                sdsMakeRoomFor(addlen - totlen);
                totlen = len + free;
            }

            memcpy(buf, t, addlen);
            buf[addlen] = '\0';
            len = addlen;
            free = totlen - len;
            
            return buf;
        }

        sds sdstrim(const char *cset){

        /*
        * 对 sds 左右两端进行修剪，清除其中 cset 指定的所有字符
        *
        * 比如 sdsstrim(xxyyabcyyxy, "xy") 将返回 "abc"
        *
        * 复杂性：
        *  T = O(M*N)，M 为 SDS 长度， N 为 cset 长度。
        */
            char *start, *end, *sp, *ep;
            size_t newlen;

            sp = start = buf;
            ep = end = buf+len-1;

            while (sp <= end && strchr(cset, *sp)) sp++;
            while (ep > start && strchr(cset, *ep)) ep--;

            newlen = (sp > ep) ? 0 : (ep-sp+1);

            if (buf != sp) memmove(buf, sp, newlen);

            buf[len] = '\0';
            free = free + len - newlen;
            len = newlen;

            return buf;
        }

        void sdsrange(int start, int end){
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
           size_t newlen;
           if (len == 0) return ;
           if (start < 0){
               start = len+start;
               if (start < 0) start = 0;
           }

           if (end < 0){
               end = len + end;
               if (end < 0) end = 0;
           }

           newlen = (start > end) ? 0 : (end - start + 1);
           if (newlen != 0){
               if (start >= (signed) len){
                   newlen = 0;
               }else if(end > (signed)len){
                   end = len-1;
                   newlen = (start > end) ? 0 : (end-start)+1;
               }
           } else{
               start = 0;
           }

           if (start && newlen) memmove(buf, buf+start, newlen);

           buf[newlen] = '\0';
           free = free + len - newlen;
           len = newlen;
        }

        
        void sdstolower() const {

            for (int i = 0; i < len; i++) buf[i] = tolower(buf[i]);            
        }

        void sdstoupper() const {

            for(int i = 0; i < len; i++) buf[i] = toupper(buf[i]);
        }

        int sdscmp(const sdshdr& s2) const{
            /*
            * 对比另一个 sds ， strcmp 的 sds 版本
            *
            * 返回值
            *  int ：相等返回 0 ，自己较大返回正数， 另一个较大返回负数
            *
            * T = O(N)
            */
            size_t len2, minlen;
            int cmp;
            len2 = s2.sdslen();
            minlen = (len < len2)? len: len2;
            cmp = memcmp(buf, s2.buf, minlen);

            if (cmp == 0) return len-len2;
            
            return cmp;
        }


        sds sdsmapchars(const char *form, const char *to, size_t setlen){
            for (size_t j = 0; j < len; j++){
                for (size_t i = 0; i < setlen; i++){
                    if (buf[j] == form[i]){
                        buf[j] = to[i];
                        break;
                    }
                }
            }
            return buf;
        }


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