/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <memory>
#include <vector>
#include <iostream>
struct Node {
    // 这两个指针会造成什么问题？请修复
    std::unique_ptr<Node> next;
    Node* prev;
    // 如果能改成 unique_ptr 就更好了!

    int value;

    // 这个构造函数有什么可以改进的？
    Node(int val) : value(val), prev(nullptr) {
    }

    void insert(int val) {
        auto node = std::make_unique<Node>(val);
        if (next) {
            next->prev = node.get();
        }
        node->next = std::move(next);
        node->prev = this;
        next = std::move(node);
    }

    void erase() {
        if (next) {
            next->prev = prev;
        }
        if (prev) {
            prev->next = std::move(next);
        }
    }

    ~Node() {
        printf("~Node()\n");   // 应输出多少次？为什么少了？
    }
};

struct List {
    std::unique_ptr<Node> head;

    List() = default;

    List(List const &other) {
        printf("List 被拷贝！\n");
        // head = other.head;  // 这是浅拷贝！
        // 请实现拷贝构造函数为 **深拷贝**
        if (!other.front()) return;
        Node *other_ptr = other.front();
        head = std::make_unique<Node>(other_ptr->value);
        auto this_ptr = head.get();

        while (other_ptr->next) {
            this_ptr->insert(other_ptr->next->value);
            this_ptr = this_ptr->next.get();
            other_ptr = other_ptr->next.get();
        }

    }

    List &operator=(List const &) = delete;  // 为什么删除拷贝赋值函数也不出错？
    // 当拷贝赋值函数被删除时，编译器会尝试通过拷贝构造函数构造新的对象，然后使用移动赋值函数完成这一操作

    List(List &&) = default;
    List &operator=(List &&) = default;

    Node *front() const {
        return head.get();
    }

    int pop_front() {
        int ret = head->value;
        head = std::move(head->next);
        return ret;
    }

    void push_front(int value) {
        auto node = std::make_unique<Node>(value);
        if (head) {
            head->prev = node.get();
            node->next = std::move(head);
        }
        head = std::move(node);
    }

    Node *at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }
};

void print(const List& lst) {  // 有什么值得改进的？
    printf("[");
    for (auto curr = lst.front(); curr; curr = curr->next.get()) {
        printf(" %d", curr->value);
    }
    printf(" ]\n");
}

void print_v2(const List& lst) {  // 有什么值得改进的？拷贝改成引用
    // printf("[");
    for (auto curr = lst.front(); curr; curr = curr->next.get()) {
        printf("curr = %d", curr->value);
        if (curr->prev) {
            std::cout << ", prev = " << curr->prev->value;
        }
        std::cout << std::endl;
    }
    // printf(" ]\n");
}

int main() {
    List a;

    a.push_front(7);
    a.push_front(5);
    a.push_front(8);
    a.push_front(2);
    a.push_front(9);
    a.push_front(4);
    a.push_front(1);

    print(a);   // [ 1 4 9 2 8 5 7 ]
    // print_v2(a);
    a.at(2)->erase();

    print(a);   // [ 1 4 2 8 5 7 ]

    List b = a;

    a.at(3)->erase();

    print(a);   // [ 1 4 2 5 7 ]
    print(b);   // [ 1 4 2 8 5 7 ]

    b = {};
    a = {};

    std::cout << "hello world" << std::endl;
    return 0;
}
