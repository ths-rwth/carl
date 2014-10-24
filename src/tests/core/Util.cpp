#pragma once

#include <list>

template<typename type>
void expectRightOrder(std::list<type>& list)
{
    int i1 = 0, i2 = 0;
    for (auto it1 : list) {
        i2 = 0;
        for (auto it2 : list) {
            EXPECT_EQ(i1 < i2, it1 < it2);
            EXPECT_EQ(i1 > i2, it1 > it2);
            EXPECT_EQ(i1 <= i2, it1 <= it2);
            EXPECT_EQ(i1 >= i2, it1 >= it2);
            EXPECT_EQ(i1 == i2, it1 == it2);
            i2++;
        }
        i1++;
    }
}
