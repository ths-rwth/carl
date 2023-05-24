#include "../Common.h"

#include <carl-arith/numbers/FactorizationFactory.h>
#include <carl-arith/numbers/numbers.h>

#include <numeric>

template<typename T>
class FactorizationFactory: public testing::Test {};

using Res = std::vector<carl::uint>;

TEST(FactorizationFactory, uint)
{
    carl::FactorizationFactory<carl::uint> factorization;
    EXPECT_EQ(Res({}), factorization(0));
    EXPECT_EQ(Res({1}), factorization(1));
    EXPECT_EQ(Res({2}), factorization(2));
    EXPECT_EQ(Res({3}), factorization(3));
    EXPECT_EQ(Res({2,2}), factorization(4));
    EXPECT_EQ(Res({5}), factorization(5));
    EXPECT_EQ(Res({2,3}), factorization(6));
    EXPECT_EQ(Res({7}), factorization(7));
    EXPECT_EQ(Res({2,2,2}), factorization(8));
    EXPECT_EQ(Res({3,3}), factorization(9));
    EXPECT_EQ(Res({2,5}), factorization(10));
    EXPECT_EQ(Res({11}), factorization(11));
    EXPECT_EQ(Res({2,2,3}), factorization(12));
    EXPECT_EQ(Res({13}), factorization(13));
    EXPECT_EQ(Res({2,7}), factorization(14));
    EXPECT_EQ(Res({3,5}), factorization(15));
    EXPECT_EQ(Res({2,2,2,2}), factorization(16));
    EXPECT_EQ(Res({17}), factorization(17));
    EXPECT_EQ(Res({2,3,3}), factorization(18));
    EXPECT_EQ(Res({19}), factorization(19));
    EXPECT_EQ(Res({2,2,5}), factorization(20));
    
    for (carl::uint i = 21; i < 500; i++) {
        auto res = factorization(i);
        auto product = std::accumulate(res.begin(), res.end(), carl::uint(1), [](auto a, auto b){ return a*b; });
        EXPECT_EQ(product, i);
    }
}
