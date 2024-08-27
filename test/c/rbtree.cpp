#define NO_STD 0
extern "C" {
#include <data-structure/rbtree.h>
}

#include <gtest/gtest.h>

TEST(RBTreeTest, InsertAndGet) {
  rbtree_t tree = null;
  rbtree_insert(tree, 10, (void *)"ten");
  rbtree_insert(tree, 20, (void *)"twenty");
  rbtree_insert(tree, 30, (void *)"thirty");

  EXPECT_EQ((char *)rbtree_get(tree, 10), std::string("ten"));
  EXPECT_EQ((char *)rbtree_get(tree, 20), std::string("twenty"));
  EXPECT_EQ((char *)rbtree_get(tree, 30), std::string("thirty"));

  rbtree_free(tree);
}

TEST(RBTreeTest, DeleteNode) {
  rbtree_t tree = null;
  rbtree_insert(tree, 10, (void *)"ten");
  rbtree_insert(tree, 20, (void *)"twenty");
  rbtree_insert(tree, 30, (void *)"thirty");

  rbtree_delete(tree, 20);
  EXPECT_EQ(rbtree_get(tree, 20), nullptr);

  rbtree_free(tree);
}

TEST(RBTreeTest, SearchNode) {
  rbtree_t tree = null;
  rbtree_insert(tree, 10, (void *)"ten");
  rbtree_insert(tree, 20, (void *)"twenty");

  int32_t key;
  EXPECT_TRUE(rbtree_search(tree, (void *)"ten", &key));
  EXPECT_EQ(key, 10);

  EXPECT_TRUE(rbtree_search(tree, (void *)"twenty", &key));
  EXPECT_EQ(key, 20);

  rbtree_free(tree);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
