#include <vector>

#include "gtest/gtest.h"
#include "mmap_std_allocator.h"

TEST(mmap_allocator, alloc)
{
    malloc(100000);
    std::vector<int> v;
    for (int i = 0; i < 1000; i++) {
        v.push_back(i);
    }

    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(v[i], i);
    }
}

TEST(mmap_allocator, free)
{
    std::vector<int, hoard::mmap_std_allocator<int>> v;
    v.~vector();
}
