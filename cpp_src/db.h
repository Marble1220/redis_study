#ifndef __DB_H__
#define __DB_H__

#include "object.h"
#include "dict.h"

#define ELIMINATE_POOL_SIZE 200


// static unsigned long maxmemory = 8589934592;
static unsigned long maxmemory = 500000000;

struct eliminatedele{
    StringObject *key;
    unsigned long idle;
};

class db{
    private:
        bool lru=1;
    public:
        dict *val, *expires;
        eliminatedele eliminatepool[ELIMINATE_POOL_SIZE]{};


    
        db();
        ~db();
        void update_eliminatepool();

        // 检验是否过期， 如果未过期或未设置过期值返回1
        // 如果已过期， 则删除对应的kv并且返回-1
        int check_expires(StringObject* key);
        // 为数据库中的一个k设置过期时间
        // 成功返回1
        // 如果key不存在， 返回0
        int set_expires(StringObject*, long long when);
        //移除一个key的过期时间
        // 成功返回1 因不存在而失败返回0
        int remove_expires(StringObject* key);
        // 返回一个key的过期时间
        // 如果key不存在, 返回0
        long long get_expires(StringObject* key);
        // 将一个key和其对应的数据结构添加到数据库中
        // 如果key已存在， 则覆盖原来的key并重置起过期时间
        // 成功返回创建的对偶指针， 失败返回nullptr
        BaseObject* set_value(StringObject* key, int object_type, BaseObject* value);
        // 得到与k对应的val， 如果key不存在返回nullptr
        BaseObject* get_value(StringObject* key);
        // 得到k对应val的类型
        // k不存在返回0
        int get_type(StringObject* key);
        // 检查对应的key是否存在于数据库, 存在返回1， 不存在返回0
        int exists(StringObject* key);
        // 删除对应的kv以及其对应的过期时间
        // 成功返回1， 因不存在失败返回0
        int delete_key(StringObject* key);
        // 控制是否打开lru， 默认为关闭, 0为关闭，1为打开
        void set_lru(int flag);

        // 使用近似的lru算法淘汰key, 返回释放内存的大小， 如果返回0则表明未开启或无需释放
        size_t lru_eliminated();
};


#endif