#include "list.h"

List::List(){
    head.pre = head.next = nullptr;
    len = 0;
}
void List::initialize(){
    head.pre = head.next = nullptr;
    len = 0;
}

int List::size(){
    return this->len;
}

bool List::empty(){
    return this->len == 0;
}

ListNode* List::front(){
    return head.next;
}
ListNode* List::back(){
    ListNode* tmp = head.next;
    if(tmp == nullptr)return nullptr;
    while(tmp->next != nullptr)tmp=tmp->next;
    return tmp;
}
void List::push_back(ListNode* node){
    if(head.next == nullptr){
        head.next = node;
        node->pre = &head;
        node->next = nullptr;
        len+=1;
        return;
    }
    ListNode* bk = this->back();
    bk->next = node;
    node->pre = bk;
    node->next = nullptr;
    len+=1;
}
void List::push_front(ListNode* node){
    if(head.next != nullptr)(head.next)->pre = node;
    node->next = head.next;
    head.next = node;
    node->pre = &head;
    len+=1;
}
void List::pop_back(){
    ListNode* bk = back();
    if(bk != nullptr){
        bk->pre->next =nullptr;
        bk->pre = bk->next = nullptr;
        len-=1;
    }
}
void List::pop_front(){
    if(head.next != nullptr){
        ListNode* tmp = head.next;
        if((head.next)->next != nullptr)(head.next)->next->pre = &head;
        head.next = (head.next)->next;
        tmp->pre = tmp->next = nullptr;
        len-=1;
    }
}
void List::insert(int pos, ListNode* node){
    if(pos < 0 || pos > len)return;
    if(pos == 0)push_front(node);
    else if(pos == len)push_back(node);
    else{
        ListNode * tmp = at(pos);

        node->pre = tmp->pre;
        node->next = tmp;
        tmp->pre->next = node;
        tmp->pre = node;
        len+=1;
    }
}
void List::erase(int pos){
    if(pos < 0 || pos >= this->len)return;

    if(pos == 0)pop_front();
    else if(pos == this->len-1)pop_back();
    else{
        ListNode* tmp = at(pos);
        tmp->pre->next = tmp->next;
        if(tmp->next != nullptr)tmp->next->pre = tmp->pre;
        tmp->pre = tmp->next = nullptr;
        len-=1;
    }
}
void List::erase(ListNode* node){
    ListNode* tmp = head.next;
    while(tmp!=nullptr && tmp != node)tmp = tmp->next;
    if(tmp != nullptr){
        tmp->pre->next = tmp->next;
        if(tmp->next != nullptr)tmp->next->pre = tmp->pre;
        tmp->pre = tmp->next = nullptr;
        len-=1;
    }
}
int List::find(ListNode* node){
    ListNode* tmp = head.next;
    int pos = 0;
    while(tmp !=nullptr && tmp != node){
        tmp = tmp->next;
        pos+=1;
    }
    if(tmp != nullptr)return pos;
    return -1;
}
ListNode* List::at(int pos){
    if(pos >=0 && pos < len){
        ListNode* tmp = head.next;
        while(pos--){
            tmp = tmp->next;
        }
        return tmp;
    }
    return nullptr;
}
