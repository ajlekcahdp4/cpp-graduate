#include <iostream>
#include <list>
#include <iterator>

template <typename T> struct node_t
{
    T data1;
    int data2;

    node_t (T data1, int data2)
    {
        this->data1 = data1;
        this->data2 = data2;
    }
};

int main ()
{
    std::list<struct node_t <int>> list;

    node_t<int> node1 (11, 1);
    node_t<int> node2 (22, 2);

    list.push_front(node1);
    list.push_front(node2);

    std::list<node_t<int>>::iterator it;

    for (it = list.begin(); it != list.end(); it++)
        std::cout << (*it).data1 << "\n";

    return 0;

}