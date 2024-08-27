#include <gtest/gtest.h>

#define NO_STD 0
#include <data-structure.hpp>

using cpp::RBTree;
using cpp::base::rbtree;

class RBTreeTest : public testing::Test {
protected:
  void SetUp() override {
    tree = new rbtree<int, int>(10, 100);
  }

  void TearDown() override {
    delete tree;
  }

  rbtree<int, int> *tree;
};

class RBTreeTest2 : public testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override {}

  RBTree<int, int> tree;
};

TEST_F(RBTreeTest, InsertTest) {
  tree = tree->insert(20, 200);
  tree = tree->insert(5, 50);

  EXPECT_EQ(*tree->get(10), 100);
  EXPECT_EQ(*tree->get(20), 200);
  EXPECT_EQ(*tree->get(5), 50);
}

TEST_F(RBTreeTest, GetTest) {
  tree = tree->insert(20, 200);
  tree = tree->insert(5, 50);

  EXPECT_EQ(*tree->get(10), 100);
  EXPECT_EQ(*tree->get(20), 200);
  EXPECT_EQ(*tree->get(5), 50);
  EXPECT_EQ(tree->get(15), nullptr); // Non-existent key
}

TEST_F(RBTreeTest, DeleteTest) {
  tree = tree->insert(20, 200);
  tree = tree->insert(5, 50);
  tree = tree->insert(15, 150);

  tree = tree->delete_(20);
  EXPECT_EQ(tree->get(20), nullptr);

  tree = tree->delete_(5);
  EXPECT_EQ(tree->get(5), nullptr);

  tree = tree->delete_(10);
  EXPECT_EQ(tree->get(10), nullptr);

  EXPECT_EQ(*tree->get(15), 150);
}

TEST_F(RBTreeTest, MinMaxTest) {
  tree = tree->insert(20, 200);
  tree = tree->insert(5, 50);
  tree = tree->insert(15, 150);

  EXPECT_EQ(*tree->min(), 50);
  EXPECT_EQ(*tree->max(), 200);
}

TEST_F(RBTreeTest, SearchTest) {
  tree = tree->insert(20, 200);
  tree = tree->insert(5, 50);
  tree = tree->insert(15, 150);

  EXPECT_EQ(*tree->search(50), 5);
  EXPECT_EQ(*tree->search(150), 15);
  EXPECT_EQ(tree->search(250), nullptr);
}

TEST_F(RBTreeTest2, InsertTest) {
  tree.insert(10, 100);
  tree.insert(20, 200);
  tree.insert(5, 50);

  EXPECT_EQ(tree.get(10), 100);
  EXPECT_EQ(tree.get(20), 200);
  EXPECT_EQ(tree.get(5), 50);
}

TEST_F(RBTreeTest2, GetTest) {
  tree.insert(10, 100);
  tree.insert(20, 200);
  tree.insert(5, 50);

  EXPECT_EQ(tree.get(10), 100);
  EXPECT_EQ(tree.get(20), 200);
  EXPECT_EQ(tree.get(5), 50);
  EXPECT_EQ(tree.get(15), 0); // Non-existent key
}

TEST_F(RBTreeTest2, DeleteTest) {
  tree.insert(10, 100);
  tree.insert(20, 200);
  tree.insert(5, 50);
  tree.insert(15, 150);

  tree.delete_(20);
  EXPECT_EQ(tree.get(20), 0);

  tree.delete_(5);
  EXPECT_EQ(tree.get(5), 0);

  tree.delete_(10);
  EXPECT_EQ(tree.get(10), 0);

  EXPECT_EQ(tree.get(15), 150);
}

TEST_F(RBTreeTest2, MinMaxTest) {
  tree.insert(10, 100);
  tree.insert(20, 200);
  tree.insert(5, 50);
  tree.insert(15, 150);

  EXPECT_EQ(*tree.min(), 50);
  EXPECT_EQ(*tree.max(), 200);
}

TEST_F(RBTreeTest2, SearchTest) {
  tree.insert(10, 100);
  tree.insert(20, 200);
  tree.insert(5, 50);
  tree.insert(15, 150);

  EXPECT_EQ(*tree.search(50), 5);
  EXPECT_EQ(*tree.search(150), 15);
  EXPECT_EQ(tree.search(250), nullptr);
}

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
