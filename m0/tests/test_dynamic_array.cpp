#include "../include/dynamic_array.hpp"

#include <iostream>
#include <utility>

namespace {

int g_failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::cout << "FAIL  " << #cond << "  (line " << __LINE__ << ")\n"; \
            ++g_failures;                                                      \
        }                                                                      \
    } while (false)

void test_empty() {
    DynamicArray a;
    CHECK(a.size() == 0);
    CHECK(a.capacity() == 0);
}

void test_push_back_grows() {
    DynamicArray a;
    a.push_back(1);
    CHECK(a.size() == 1);
    CHECK(a.capacity() == 1);

    a.push_back(2);
    CHECK(a.capacity() == 2);

    a.push_back(3);
    CHECK(a.size() == 3);
    CHECK(a.capacity() == 4);
}

void test_grow_keeps_data() {
    DynamicArray a;
    for (int i = 1; i <= 10; ++i) {
        a.push_back(i);
    }

    CHECK(a.size() == 10);
    for (std::size_t i = 0; i < a.size(); ++i) {
        CHECK(a[i] == static_cast<int>(i + 1));
    }
}

void test_copy_constructor_is_deep() {
    DynamicArray a;
    a.push_back(1);
    a.push_back(2);

    DynamicArray b = a;
    b[0] = 100;

    CHECK(a[0] == 1);
    CHECK(b[0] == 100);
    CHECK(a[1] == 2);
    CHECK(b[1] == 2);
}

void test_copy_assignment_is_deep() {
    DynamicArray a;
    a.push_back(1);
    a.push_back(2);

    DynamicArray b;
    b.push_back(9);
    b = a;
    b[1] = 200;

    CHECK(a[1] == 2);
    CHECK(b[1] == 200);
}

void test_self_assignment() {
    DynamicArray a;
    a.push_back(7);

    a = a;

    CHECK(a.size() == 1);
    CHECK(a[0] == 7);
}

void test_move_constructor() {
    DynamicArray a;
    a.push_back(1);
    a.push_back(2);

    DynamicArray b = std::move(a);

    CHECK(a.size() == 0);
    CHECK(a.capacity() == 0);
    CHECK(b.size() == 2);
    CHECK(b[0] == 1);
    CHECK(b[1] == 2);
}

void test_move_assignment() {
    DynamicArray a;
    a.push_back(1);
    a.push_back(2);

    DynamicArray b;
    b.push_back(9);

    b = std::move(a);

    CHECK(a.size() == 0);
    CHECK(a.capacity() == 0);
    CHECK(b.size() == 2);
    CHECK(b[0] == 1);
    CHECK(b[1] == 2);
}

void test_destructor_no_crash() {
    DynamicArray a;
    a.push_back(1);
    DynamicArray b = a;
    b.push_back(2);
    DynamicArray c = std::move(b);
    c.push_back(3);
}

void test_copy_assignment_strong_guarantee() {
    DynamicArray a;
    a.push_back(1);
    a.push_back(2);
    DynamicArray b;
    b.push_back(7);

    // 用一个必然失败的容量触发 bad_alloc
    DynamicArray huge;
    // 技巧：先构造一个 capacity 极大的源对象不可行（会真的分配），
    // 更实际的做法是把这条测试记为"已理解但无法在栈上构造"，
    // 或者把 new 换成可注入的分配器（M2 之后再考虑）。
    // 现阶段的最低要求：把"为什么测不了"写进 docs，而不是假装覆盖了。
    //已理解但无法在栈上构造
    //因为要触发bad_malloc需要分配很大的内存
}

}  // namespace

int main() {
    test_empty();
    test_push_back_grows();
    test_grow_keeps_data();
    test_copy_constructor_is_deep();
    test_copy_assignment_is_deep();
    test_self_assignment();
    test_move_constructor();
    test_move_assignment();
    test_destructor_no_crash();
    test_copy_assignment_strong_guarantee();

    if (g_failures == 0) {
        std::cout << "test_dynamic_array passed\n";
        return 0;
    }

    std::cout << "test_dynamic_array FAILED: " << g_failures << " check(s)\n";
    return 1;
}