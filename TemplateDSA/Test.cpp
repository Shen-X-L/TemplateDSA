#include <iostream>
#include <vector>
#include <map>
#include <array>

#define TEST_ARRAY
#define TEST

#ifdef TEST_HEAP
#include "Heap.hpp"
#endif

#ifdef TEST_LINKED_LIST
#include "LinkedList/LinkedList.hpp"
#endif

#ifdef TEST_ARRAY
#include "List/Array.hpp"
#endif

int main() {
#ifdef TEST_HEAP
    return test_heap();
#endif
#ifdef TEST_LINKED_LIST
    return test_linked_list();
#endif
#ifdef TEST_ARRAY
    return test_array();
#endif
}


