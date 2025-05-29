#include <iostream>
#include <forward_list>
int main() {
    std::forward_list<int> l = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    l.erase_after(l.before_begin()); // ÒÆ³ıÊ×ÔªËØ

    for (auto n : l)
        std::cout << n << ' ';
    std::cout << '\n';

    l.erase_after(l.erase_after(l.before_begin()));

    for (auto n : l)
        std::cout << n << ' ';
    std::cout << '\n';
}


