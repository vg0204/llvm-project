//===- unittest/ADT/MapVectorTest.cpp - MapVector unit tests ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/iterator_range.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>
#include <utility>

using namespace llvm;

namespace {
struct CountCopyAndMove {
  CountCopyAndMove() = default;
  CountCopyAndMove(const CountCopyAndMove &) { copy = 1; }
  CountCopyAndMove(CountCopyAndMove &&) { move = 1; }
  void operator=(const CountCopyAndMove &) { ++copy; }
  void operator=(CountCopyAndMove &&) { ++move; }
  int copy = 0;
  int move = 0;
};

struct A : CountCopyAndMove {
  A(int v) : v(v) {}
  int v;
};
} // namespace

namespace llvm {
template <> struct DenseMapInfo<A> {
  static inline A getEmptyKey() { return 0x7fffffff; }
  static inline A getTombstoneKey() { return -0x7fffffff - 1; }
  static unsigned getHashValue(const A &Val) { return (unsigned)(Val.v * 37U); }
  static bool isEqual(const A &LHS, const A &RHS) { return LHS.v == RHS.v; }
};
} // namespace llvm

namespace {
TEST(MapVectorTest, swap) {
  MapVector<int, int> MV1, MV2;
  std::pair<MapVector<int, int>::iterator, bool> R;

  R = MV1.insert(std::make_pair(1, 2));
  ASSERT_EQ(R.first, MV1.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_TRUE(R.second);

  EXPECT_FALSE(MV1.empty());
  EXPECT_TRUE(MV2.empty());
  MV2.swap(MV1);
  EXPECT_TRUE(MV1.empty());
  EXPECT_FALSE(MV2.empty());

  auto I = MV1.find(1);
  ASSERT_EQ(MV1.end(), I);

  I = MV2.find(1);
  ASSERT_EQ(I, MV2.begin());
  EXPECT_EQ(I->first, 1);
  EXPECT_EQ(I->second, 2);
}

TEST(MapVectorTest, insert_pop) {
  MapVector<int, int> MV;
  std::pair<MapVector<int, int>::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_TRUE(R.second);

  R = MV.insert(std::make_pair(1, 3));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_FALSE(R.second);

  R = MV.insert(std::make_pair(4, 5));
  ASSERT_NE(R.first, MV.end());
  EXPECT_EQ(R.first->first, 4);
  EXPECT_EQ(R.first->second, 5);
  EXPECT_TRUE(R.second);

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 5);

  MV.pop_back();
  EXPECT_EQ(MV.size(), 1u);
  EXPECT_EQ(MV[1], 2);

  R = MV.insert(std::make_pair(4, 7));
  ASSERT_NE(R.first, MV.end());
  EXPECT_EQ(R.first->first, 4);
  EXPECT_EQ(R.first->second, 7);
  EXPECT_TRUE(R.second);

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 7);
}

TEST(MapVectorTest, try_emplace) {
  struct AAndU {
    A a;
    std::unique_ptr<int> b;
    AAndU(A a, std::unique_ptr<int> b) : a(a), b(std::move(b)) {}
  };
  MapVector<A, AAndU> mv;

  A zero(0);
  auto try0 = mv.try_emplace(zero, zero, nullptr);
  EXPECT_TRUE(try0.second);
  EXPECT_EQ(0, try0.first->second.a.v);
  EXPECT_EQ(1, try0.first->second.a.copy);
  EXPECT_EQ(0, try0.first->second.a.move);

  auto try1 = mv.try_emplace(zero, zero, nullptr);
  EXPECT_FALSE(try1.second);
  EXPECT_EQ(0, try1.first->second.a.v);
  EXPECT_EQ(1, try1.first->second.a.copy);
  EXPECT_EQ(0, try1.first->second.a.move);

  EXPECT_EQ(try0.first, try1.first);
  EXPECT_EQ(1, try1.first->first.copy);
  EXPECT_EQ(0, try1.first->first.move);

  A two(2);
  auto try2 = mv.try_emplace(2, std::move(two), std::make_unique<int>(2));
  EXPECT_TRUE(try2.second);
  EXPECT_EQ(2, try2.first->second.a.v);
  EXPECT_EQ(0, try2.first->second.a.move);

  std::unique_ptr<int> p(new int(3));
  auto try3 = mv.try_emplace(std::move(two), 3, std::move(p));
  EXPECT_FALSE(try3.second);
  EXPECT_EQ(2, try3.first->second.a.v);
  EXPECT_EQ(1, try3.first->second.a.copy);
  EXPECT_EQ(0, try3.first->second.a.move);

  EXPECT_EQ(try2.first, try3.first);
  EXPECT_EQ(0, try3.first->first.copy);
  EXPECT_EQ(1, try3.first->first.move);
  EXPECT_NE(nullptr, p);
}

TEST(MapVectorTest, insert_or_assign) {
  MapVector<A, A> mv;

  A zero(0);
  auto try0 = mv.insert_or_assign(zero, zero);
  EXPECT_TRUE(try0.second);
  EXPECT_EQ(0, try0.first->second.v);
  EXPECT_EQ(1, try0.first->second.copy);
  EXPECT_EQ(0, try0.first->second.move);

  auto try1 = mv.insert_or_assign(zero, zero);
  EXPECT_FALSE(try1.second);
  EXPECT_EQ(0, try1.first->second.v);
  EXPECT_EQ(2, try1.first->second.copy);
  EXPECT_EQ(0, try1.first->second.move);

  EXPECT_EQ(try0.first, try1.first);
  EXPECT_EQ(1, try1.first->first.copy);
  EXPECT_EQ(0, try1.first->first.move);

  A two(2);
  auto try2 = mv.try_emplace(2, std::move(two));
  EXPECT_TRUE(try2.second);
  EXPECT_EQ(2, try2.first->second.v);
  EXPECT_EQ(1, try2.first->second.move);

  auto try3 = mv.insert_or_assign(std::move(two), 3);
  EXPECT_FALSE(try3.second);
  EXPECT_EQ(3, try3.first->second.v);
  EXPECT_EQ(0, try3.first->second.copy);
  EXPECT_EQ(2, try3.first->second.move);

  EXPECT_EQ(try2.first, try3.first);
  EXPECT_EQ(0, try3.first->first.copy);
  EXPECT_EQ(1, try3.first->first.move);
}

TEST(MapVectorTest, erase) {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 2));
  MV.insert(std::make_pair(3, 4));
  MV.insert(std::make_pair(5, 6));
  ASSERT_EQ(MV.size(), 3u);

  ASSERT_TRUE(MV.contains(1));
  MV.erase(MV.find(1));
  ASSERT_EQ(MV.size(), 2u);
  ASSERT_FALSE(MV.contains(1));
  ASSERT_EQ(MV.find(1), MV.end());
  ASSERT_EQ(MV[3], 4);
  ASSERT_EQ(MV[5], 6);

  ASSERT_EQ(MV.erase(3), 1u);
  ASSERT_EQ(MV.size(), 1u);
  ASSERT_EQ(MV.find(3), MV.end());
  ASSERT_EQ(MV[5], 6);

  ASSERT_EQ(MV.erase(79), 0u);
  ASSERT_EQ(MV.size(), 1u);
}

TEST(MapVectorTest, remove_if) {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  ASSERT_EQ(MV.size(), 6u);

  MV.remove_if([](const std::pair<int, int> &Val) { return Val.second % 2; });
  ASSERT_EQ(MV.size(), 3u);
  ASSERT_EQ(MV.find(1), MV.end());
  ASSERT_EQ(MV.find(3), MV.end());
  ASSERT_EQ(MV.find(5), MV.end());
  ASSERT_EQ(MV[2], 12);
  ASSERT_EQ(MV[4], 14);
  ASSERT_EQ(MV[6], 16);
}

TEST(MapVectorTest, iteration_test) {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  ASSERT_EQ(MV.size(), 6u);

  int count = 1;
  for (auto P : make_range(MV.begin(), MV.end())) {
    ASSERT_EQ(P.first, count);
    count++;
  }

  count = 6;
  for (auto P : make_range(MV.rbegin(), MV.rend())) {
    ASSERT_EQ(P.first, count);
    count--;
  }
}

TEST(MapVectorTest, NonCopyable) {
  MapVector<int, std::unique_ptr<int>> MV;
  MV.insert(std::make_pair(1, std::make_unique<int>(1)));
  MV.insert(std::make_pair(2, std::make_unique<int>(2)));

  ASSERT_EQ(MV.count(1), 1u);
  ASSERT_EQ(*MV.find(2)->second, 2);
}

TEST(MapVectorTest, GetArrayRef) {
  MapVector<int, int> MV;

  // The underlying vector is empty to begin with.
  EXPECT_TRUE(MV.getArrayRef().empty());

  // Test inserted element.
  MV.insert(std::make_pair(100, 99));
  EXPECT_TRUE(MV.getArrayRef().equals({std::pair(100, 99)}));

  // Inserting a different element for an existing key won't change the
  // underlying vector.
  auto [Iter, Inserted] = MV.try_emplace(100, 98);
  EXPECT_FALSE(Inserted);
  EXPECT_EQ(Iter->second, 99);
  EXPECT_TRUE(MV.getArrayRef().equals({std::pair(100, 99)}));

  // Inserting a new element. Tests that elements are in order in the underlying
  // array.
  MV.insert(std::make_pair(99, 98));
  EXPECT_TRUE(MV.getArrayRef().equals({std::pair(100, 99), std::pair(99, 98)}));
}

template <class IntType> struct MapVectorMappedTypeTest : ::testing::Test {
  using int_type = IntType;
};

using MapIntTypes = ::testing::Types<int, long, long long, unsigned,
                                     unsigned long, unsigned long long>;
TYPED_TEST_SUITE(MapVectorMappedTypeTest, MapIntTypes, );

TYPED_TEST(MapVectorMappedTypeTest, DifferentDenseMap) {
  // Test that using a map with a mapped type other than 'unsigned' compiles
  // and works.
  using IntType = typename TestFixture::int_type;
  using MapVectorType = MapVector<int, int, DenseMap<int, IntType>>;

  MapVectorType MV;
  std::pair<typename MapVectorType::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_TRUE(R.second);

  const std::pair<int, int> Elem(1, 3);
  R = MV.insert(Elem);
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_FALSE(R.second);

  int& value = MV[4];
  EXPECT_EQ(value, 0);
  value = 5;

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 5);
}

TEST(SmallMapVectorSmallTest, insert_pop) {
  SmallMapVector<int, int, 32> MV;
  std::pair<SmallMapVector<int, int, 32>::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_TRUE(R.second);

  R = MV.insert(std::make_pair(1, 3));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_FALSE(R.second);

  R = MV.insert(std::make_pair(4, 5));
  ASSERT_NE(R.first, MV.end());
  EXPECT_EQ(R.first->first, 4);
  EXPECT_EQ(R.first->second, 5);
  EXPECT_TRUE(R.second);

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 5);

  MV.pop_back();
  EXPECT_EQ(MV.size(), 1u);
  EXPECT_EQ(MV[1], 2);

  R = MV.insert(std::make_pair(4, 7));
  ASSERT_NE(R.first, MV.end());
  EXPECT_EQ(R.first->first, 4);
  EXPECT_EQ(R.first->second, 7);
  EXPECT_TRUE(R.second);

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 7);
}

TEST(SmallMapVectorSmallTest, erase) {
  SmallMapVector<int, int, 32> MV;

  MV.insert(std::make_pair(1, 2));
  MV.insert(std::make_pair(3, 4));
  MV.insert(std::make_pair(5, 6));
  ASSERT_EQ(MV.size(), 3u);

  MV.erase(MV.find(1));
  ASSERT_EQ(MV.size(), 2u);
  ASSERT_EQ(MV.find(1), MV.end());
  ASSERT_EQ(MV[3], 4);
  ASSERT_EQ(MV[5], 6);

  ASSERT_EQ(MV.erase(3), 1u);
  ASSERT_EQ(MV.size(), 1u);
  ASSERT_EQ(MV.find(3), MV.end());
  ASSERT_EQ(MV[5], 6);

  ASSERT_EQ(MV.erase(79), 0u);
  ASSERT_EQ(MV.size(), 1u);
}

TEST(SmallMapVectorSmallTest, remove_if) {
  SmallMapVector<int, int, 32> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  ASSERT_EQ(MV.size(), 6u);

  MV.remove_if([](const std::pair<int, int> &Val) { return Val.second % 2; });
  ASSERT_EQ(MV.size(), 3u);
  ASSERT_EQ(MV.find(1), MV.end());
  ASSERT_EQ(MV.find(3), MV.end());
  ASSERT_EQ(MV.find(5), MV.end());
  ASSERT_EQ(MV[2], 12);
  ASSERT_EQ(MV[4], 14);
  ASSERT_EQ(MV[6], 16);
}

TEST(SmallMapVectorSmallTest, iteration_test) {
  SmallMapVector<int, int, 32> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  ASSERT_EQ(MV.size(), 6u);

  int count = 1;
  for (auto P : make_range(MV.begin(), MV.end())) {
    ASSERT_EQ(P.first, count);
    count++;
  }

  count = 6;
  for (auto P : make_range(MV.rbegin(), MV.rend())) {
    ASSERT_EQ(P.first, count);
    count--;
  }
}

TEST(SmallMapVectorSmallTest, NonCopyable) {
  SmallMapVector<int, std::unique_ptr<int>, 8> MV;
  MV.insert(std::make_pair(1, std::make_unique<int>(1)));
  MV.insert(std::make_pair(2, std::make_unique<int>(2)));

  ASSERT_EQ(MV.count(1), 1u);
  ASSERT_EQ(*MV.find(2)->second, 2);
}

TEST(SmallMapVectorLargeTest, insert_pop) {
  SmallMapVector<int, int, 1> MV;
  std::pair<SmallMapVector<int, int, 1>::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_TRUE(R.second);

  R = MV.insert(std::make_pair(1, 3));
  ASSERT_EQ(R.first, MV.begin());
  EXPECT_EQ(R.first->first, 1);
  EXPECT_EQ(R.first->second, 2);
  EXPECT_FALSE(R.second);

  R = MV.insert(std::make_pair(4, 5));
  ASSERT_NE(R.first, MV.end());
  EXPECT_EQ(R.first->first, 4);
  EXPECT_EQ(R.first->second, 5);
  EXPECT_TRUE(R.second);

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 5);

  MV.pop_back();
  EXPECT_EQ(MV.size(), 1u);
  EXPECT_EQ(MV[1], 2);

  R = MV.insert(std::make_pair(4, 7));
  ASSERT_NE(R.first, MV.end());
  EXPECT_EQ(R.first->first, 4);
  EXPECT_EQ(R.first->second, 7);
  EXPECT_TRUE(R.second);

  EXPECT_EQ(MV.size(), 2u);
  EXPECT_EQ(MV[1], 2);
  EXPECT_EQ(MV[4], 7);
}

TEST(SmallMapVectorLargeTest, erase) {
  SmallMapVector<int, int, 1> MV;

  MV.insert(std::make_pair(1, 2));
  MV.insert(std::make_pair(3, 4));
  MV.insert(std::make_pair(5, 6));
  ASSERT_EQ(MV.size(), 3u);

  MV.erase(MV.find(1));
  ASSERT_EQ(MV.size(), 2u);
  ASSERT_EQ(MV.find(1), MV.end());
  ASSERT_EQ(MV[3], 4);
  ASSERT_EQ(MV[5], 6);

  ASSERT_EQ(MV.erase(3), 1u);
  ASSERT_EQ(MV.size(), 1u);
  ASSERT_EQ(MV.find(3), MV.end());
  ASSERT_EQ(MV[5], 6);

  ASSERT_EQ(MV.erase(79), 0u);
  ASSERT_EQ(MV.size(), 1u);
}

TEST(SmallMapVectorLargeTest, remove_if) {
  SmallMapVector<int, int, 1> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  ASSERT_EQ(MV.size(), 6u);

  MV.remove_if([](const std::pair<int, int> &Val) { return Val.second % 2; });
  ASSERT_EQ(MV.size(), 3u);
  ASSERT_EQ(MV.find(1), MV.end());
  ASSERT_EQ(MV.find(3), MV.end());
  ASSERT_EQ(MV.find(5), MV.end());
  ASSERT_EQ(MV[2], 12);
  ASSERT_EQ(MV[4], 14);
  ASSERT_EQ(MV[6], 16);
}

TEST(SmallMapVectorLargeTest, iteration_test) {
  SmallMapVector<int, int, 1> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  ASSERT_EQ(MV.size(), 6u);

  int count = 1;
  for (auto P : make_range(MV.begin(), MV.end())) {
    ASSERT_EQ(P.first, count);
    count++;
  }

  count = 6;
  for (auto P : make_range(MV.rbegin(), MV.rend())) {
    ASSERT_EQ(P.first, count);
    count--;
  }
}
} // namespace
