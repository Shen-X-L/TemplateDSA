#include <iostream>
#include <string>
#include <array>
#include"List/LinearList.hpp"
#include"List/Array.hpp"

int main() {
    int a = 1, b = 2;
    std::array<int, 5> c{};
    sxl::Array<int, 5> d{1,2,3,4,5};
    std::cout << d[1];
    std::cout << __cplusplus << std::endl;
    std::swap(a,b);
    return 0;
}