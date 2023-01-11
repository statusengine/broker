#pragma once

#include <map>

template <typename T> void clearContainer(T *container) {
    for (auto it = container->begin(); it != container->end();) {
        delete *it;
        it = container->erase(it);
    }
}

template <typename T, typename A>
void clearContainer(std::map<T, A> *container) {
    for (auto it = container->begin(); it != container->end();) {
        delete it->second;
        it = container->erase(it);
    }
}

#define FRIEND_TEST(test_case_name, test_name)                                 \
    friend class test_case_name##_##test_name##_Test
