#ifndef __ADLIST_H__
#define __ADLIST_H__

#include <list>

#include "base.h"
#include "object.h"


using std::list;

// typedef list<BaseObject*>::iterator iterator;

// class adlistIter{
//     friend class adlist;
//     friend bool operator==(adlistIter &lhs, adlistIter &rhs);

//     private:
//         iterator iter;
//     public:
//         adlistIter(iterator i): iter(i){};

//         adlistIter& operator++(){
//             ++iter;
//             return *this;
//         }
//         adlistIter& operator--(){
//             --iter;
//             return *this;
//         }
//         adlistIter& operator++(int){
//             adlistIter res = *this;
//             ++*this;
//             return res;
//         }
//         adlistIter& operator--(int){
//             adlistIter res = *this;
//             --*this;
//             return res;
//         }


//         BaseObject* operator*(){
//             return *iter;
//         }
// };



// 简单封装stl的list， 我寻思比我自己实现的可靠性和效率高多了


class adlist: public BaseStruct{
    private:
        list<BaseObject*> *lst;

    public:
        typedef list<BaseObject*>::iterator adlistIter;
        adlist():BaseStruct(ADLISTSTRUCT){lst = new list<BaseObject*>();};
        ~adlist(){
            for(auto &item: *lst){
                cout << "del: " << *(item->get_value()) << endl;
                delete item;
            }
            delete lst;
        };

        adlist* adlistAddNodeHead(BaseObject *value);

        adlist* adlistAddNodeTail(BaseObject *value);

        adlist* adlistInsertNode(adlistIter old_node, BaseObject *value, int after);

        void adlistDelNode(adlistIter node);

        size_t length() const;



        adlistIter adlistGetBegin();
        
        adlistIter adlistGetEnd();
        adlistIter adlistSearch(BaseObject *key);
};





#endif
