#include "adlist.h"


adlist* adlist::adlistAddNodeHead(BaseObject *value){
    lst->insert(lst->begin(), value);
    return this;
}

adlist* adlist::adlistAddNodeTail(BaseObject *value){
    lst->push_back(value);
    return this;
}

adlist* adlist::adlistInsertNode(adlistIter old_node, BaseObject *value, int after){
    //  * 如果 after 为 0 ，将新节点插入到 old_node 之前。
    // * 如果 after 为 1 ，将新节点插入到 old_node 之后。
    if (after){
        ++old_node;
    }
    lst->insert(old_node, value);
    return this;
}

void adlist::adlistDelNode(adlistIter node){
    //  * 对节点私有值(private value of the node)的释放工作由调用者进行。
    lst->erase(node);
}

adlist::adlistIter adlist::adlistGetBegin(){
    cout << "get begin" << endl;
    return lst->begin();
}

adlist::adlistIter adlist::adlistGetEnd(){
    return lst->end();
}

size_t adlist::length() const{
    return lst->size();
}

adlist::adlistIter adlist::adlistSearch(BaseObject *key){
    // 如果找到 返回对应的adlistiter
    // 否则返回end的adlistiter
    for (auto iter = lst->begin(); iter != lst->end(); iter++){
        cout << "searchtest: " << *((*iter)->get_value()) << endl;
        if ((*iter)->match(key) == 0){
            return adlistIter(iter);
        }
    }
    return lst->end();            
}

