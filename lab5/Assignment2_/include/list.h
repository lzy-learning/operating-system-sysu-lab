#ifndef LIST_H
#define LIST_H

// 标识线程在线程队列中的位置
// 节点没有数据域，线程会根据自己保存的那个Listode找到自己在线程队列中的位置
struct ListNode{
    ListNode* pre;
    ListNode* next;
};

// 双向链表
class List{
public:
    List();
    ListNode head;  // 虚拟头节点
    int len;
    void initialize();  // 初始化
    int size();     // 返回元素个数
    bool empty();   // 返回List是否为空
    ListNode* front();
    ListNode* back();
    void push_back(ListNode*);
    void push_front(ListNode*);
    void pop_back();
    void pop_front();
    void insert(int, ListNode*);
    void erase(int);
    void erase(ListNode*);
    
    int find(ListNode*);
    ListNode* at(int);
};
#endif