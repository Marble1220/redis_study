#ifndef __ADLIST_H__
#define __ADLIST_H__
#endif

#include <list>

#include "base.h"
#include "object.h"


using std::list;
// 简单封装stl的list， 我寻思比我自己实现的可靠性和效率高多了

typedef list<BaseObject*>::iterator iterator;

class adlistIter{
    friend class adlist;
    friend bool operator==(adlistIter &lhs, adlistIter &rhs);

    private:
        iterator iter;
    public:
        adlistIter(iterator i): iter(i){};

        adlistIter& operator++(){
            ++iter;
            return *this;
        }
        adlistIter& operator--(){
            --iter;
            return *this;
        }
        adlistIter& operator++(int){
            adlistIter res = *this;
            ++*this;
            return res;
        }
        adlistIter& operator--(int){
            adlistIter res = *this;
            --*this;
            return res;
        }


        BaseObject* operator*(){
            return *iter;
        }
};

bool operator==(adlistIter &lhs, adlistIter &rhs){
    return lhs.iter == rhs.iter;
}



class adlist: public BaseStruct{
    private:
        list<BaseObject*> *lst;

    public:
        adlist(){lst = new list<BaseObject*>;};
        ~adlist(){
            for(auto &item: *lst){
                delete item;
            }
            delete lst;
        };

        adlist* adlistAddNodeHead(BaseObject *value){
            lst->insert(lst->begin(), value);
            return this;
        }

        adlist* adlistAddNodeTail(BaseObject *value){
            lst->push_back(value);
            return this;
        }

        adlist* adlistInsertNode(adlistIter old_node, BaseObject *value, int after){
            //  * 如果 after 为 0 ，将新节点插入到 old_node 之前。
            // * 如果 after 为 1 ，将新节点插入到 old_node 之后。
            if (after){
                ++old_node;
            }
            lst->insert(old_node.iter, value);
            return this;
        }

        void adlistDelNode(adlistIter node){
            //  * 对节点私有值(private value of the node)的释放工作由调用者进行。
            lst->erase(node.iter);
        }


        adlistIter adlistGetBegin(){
            return adlistIter(lst->begin());
        }
        
        adlistIter adlistGetEnd(){
            return adlistIter(lst->end());
        }

        adlistIter adlistSearch(BaseObject *key){
            // 如果找到 返回对应的adlistiter
            // 否则返回end的adlistiter
            for (auto iter = lst->begin(); iter != lst->end(); iter++){
                if ((*iter)->match(key)){
                    return adlistIter(iter);
                }
            }
            return adlistIter(lst->end());            

        }











}