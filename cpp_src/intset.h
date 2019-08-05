#ifndef __INTSET_H
#define __INTSET_H

#include <stdint.h>
#include <sys/types.h>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

#include "base.h"

#define INTSET_ENC_INT16 (sizeof(int16_t))
#define INTSET_ENC_INT32 (sizeof(int32_t))
#define INTSET_ENC_INT64 (sizeof(int64_t))


using std::vector;
using std::allocator;
using std::cout;
using std::endl;
using std::lower_bound;
using std::sort;


static uint8_t _intsetValueEncoding(int64_t v){
    if (v < INT32_MIN || v > INT32_MAX){
        return INTSET_ENC_INT64;
    }
    else if (v < INT16_MIN || v > INT16_MAX){
        return INTSET_ENC_INT32;
    }
    else{
        return INTSET_ENC_INT16;
    }
}


class ArrayBase{

    public:
        // ArrayBase() = default;
        virtual ~ArrayBase() = default;
        virtual int64_t operator[](int pos) const = 0;
        // virtual int64_t& operator[](size_t pos) = 0;
        virtual int get_len() = 0;
        virtual int get_cap() = 0;
        
        
        virtual void resize(int newlen) = 0;
        virtual int search(int64_t value, int *pos) = 0;
        virtual int add(int64_t value) = 0;
        virtual int remove(int64_t value) = 0;
        virtual int byte_size() const = 0;
        virtual void set(int pos, int64_t value)=0;
        
        virtual void test() const = 0;

};




template  <typename T> class ArrayInt: public ArrayBase{
    friend class intset;
    friend int intsetmatch(void* ptr, void* key);
    private:
        typedef vector<T> v;
        v *vec;
    
    public:
        ArrayInt():ArrayBase(){
            vec = new v();
        }

        ArrayInt(const T* initarr, int initlen){
            vec = new v(initlen);
            
            // vector<int16_t> &temp = *vec;

            auto begin = vec->begin();

            for (int i=0; i < initlen; i++){
                *begin = initarr[i];
                begin ++;
            }

            sort(vec->begin(), vec->end());
            
        }

        int get_len() {return vec->size();};
        int get_cap() {return vec->capacity();};
        void set(int pos, int64_t value){(*vec)[pos]=value;};

        int search(int64_t value, int *pos){
            // * 成功找到 value 时，函数返回 1 ，并将 *pos 的值设为 value 所在的索引。
            // * 当在数组中没找到 value 时，返回 0 。
            // * 并将 *pos 的值设为 value 可以插入到数组中的位置。
            auto begin = vec->begin();
            auto end = vec->end();
            auto ptr = lower_bound(begin, end, value);
            if (ptr == end){
                *pos = vec->size();
                return 0;
            }
            else{
                *pos = ptr - begin;
                if (*ptr == value){
                    return 1;
                }
                else{
                    return 0;
                }
            }
        }

        void resize(int newlen){
            if (newlen == vec->capacity())
                return;
            else if (newlen > vec->capacity()){
                vec->reserve(newlen);
                return ;
            }
            else{
                v *temp;
                if (newlen > vec->size()){
                    temp = new v(vec->begin(), vec->end());
                    temp->reserve(newlen);
                }
                else{
                    temp = new v(vec->begin(), vec->begin()+newlen);
                }
                delete vec;
                vec = temp;
                return ;
            }
        }

        int add(int64_t value){
            // 添加成功返回1
            // 因已存在添加失败返回0;

            auto begin = vec->begin();
            auto end = vec->end();
            auto insert_ptr = lower_bound(begin, end, value);

            if (insert_ptr == end){
                vec->push_back(value);
                return 1;
            }
            else{
                if (*insert_ptr == value)
                    return 0;
                else{
                    vec->insert(insert_ptr, value);
                    return 1;
                }
            }
        }

        int remove(int64_t value){
            // 因值不存在而造成删除失败时返回 0 。
            // 删除成功时返回 1
            int i;
             if (search(value, &i)){
                 vec->erase(vec->begin()+i);
                 return 1;
             }
             return 0;
        }

        int byte_size() const {
            return sizeof(*this) + sizeof(*vec) + sizeof(T)*vec->capacity();
        }


        void test() const {
            cout << vec->size() << " " << vec->capacity() << ' ' << byte_size() << ' ';
            cout << "{";
            for(auto &i: *vec)
                cout << i << " ";
            cout << "}";
            cout << endl;
        }


        int64_t operator[](int pos) const {return (*vec)[pos];};
        // int64_t& operator[](size_t pos) {return (*vec)[pos];};
        ~ArrayInt(){delete vec;};
};



class intset: public BaseStruct{
    friend int intsetmatch(void*, void*);
    private:
        ArrayBase* contents;
        uint32_t encoding;
        
    public:
        intset(): BaseStruct(INTSETSTRUCT), encoding(INTSET_ENC_INT16), contents(new ArrayInt<uint16_t>()) {} ;

        ~intset(){delete contents;};
        
        
        intset* intsetUpgrade(uint32_t newencoding){
            ArrayBase *temp;
            if (newencoding == INTSET_ENC_INT32){
                temp = new ArrayInt<int32_t>();
            }
            else{
                temp = new ArrayInt<int64_t>();
            }

            for(int i=0; i < contents->get_len(); i++){
                temp->add(contents->operator[](i));
            }
            delete contents;
            contents = temp;
            encoding = newencoding;
            return this;
        }

        void check(int64_t value){
            if (_intsetValueEncoding(value) > encoding)
                intsetUpgrade(_intsetValueEncoding(value));
        }
        
        
        intset* intsetResize(int newlen){
            contents->resize(newlen);
            return this;
        }

        void intsetSet(int pos, int64_t value){
            if (pos < 0) pos = contents->get_len() + pos;
            if (pos < 0 || pos >= contents->get_len()){
                throw std::range_error("out of range");
            }
            check(value);
            contents->set(pos, value);
        }

        int intsetSearch(int64_t value, int *pos){
            return contents->search(value, pos);
        }

        intset* intsetAdd(int64_t value, int* success){
            // * 尝试将元素 value 添加到整数集合中。
            // *success 的值指示添加是否成功：
            // * - 如果添加成功，那么将 *success 的值设为 1 。
            // * - 因为元素已存在而造成添加失败时，将 *success 的值设为 0 。
            check(value);
            *success = contents->add(value);
            return this;
        }

        intset *intsetRemove(int64_t value, int *success){
            uint32_t valenc = _intsetValueEncoding(value);
            if (valenc > encoding) *success = 0;
            else *success = contents->remove(value);
            return this;
        }

        uint8_t intsetFind(int64_t value){
            uint8_t valenc = _intsetValueEncoding(value);
            int i;
            return valenc <= encoding && contents->search(value, &i);
        }


        size_t intsetBlobLen() const {
            return sizeof(*this) + contents->byte_size();
        }
        
        size_t length() const{
            return contents->get_len();
        }

        int64_t operator[](int pos) const;

        void test() const {
            cout << contents->get_len() << " " << contents->get_cap() << ' ' << contents->byte_size() << ' ';
            cout << "{";
            for (int i = 0; i < length(); i++)
                cout << (*this)[i] << " ";
            cout << "}" << " ";
            cout << encoding;
        
            cout << endl;
        }





};


int intsetmatch(void*, void*);


// class ArrayInt32: public ArrayBase{};

// class ArrayInt64: public ArrayBase{};


#endif
