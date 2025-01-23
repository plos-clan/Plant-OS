#include <gtest/gtest.h>

#define NO_STD 0
#include <data-structure/unordered-map/st.h>

TEST(SymbolTableTest, CreateAndFree) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);
  st_free(st);
}

TEST(SymbolTableTest, InsertAndGet) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);

  int value = 42;
  ASSERT_EQ(st_insert(st, "key", &value), 0);
  ASSERT_EQ(*(int *)st_get(st, "key"), value);

  st_free(st);
}

TEST(SymbolTableTest, InsertAndHas) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);

  int value = 42;
  ASSERT_EQ(st_insert(st, "key", &value), 0);
  ASSERT_TRUE(st_has(st, "key"));

  st_free(st);
}

TEST(SymbolTableTest, InsertAndRemove) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);

  int value = 42;
  ASSERT_EQ(st_insert(st, "key", &value), 0);
  ASSERT_EQ(*(int *)st_remove(st, "key"), value);
  ASSERT_FALSE(st_has(st, "key"));

  st_free(st);
}

TEST(SymbolTableTest, InsertAndSet) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);

  int value1 = 42;
  int value2 = 84;
  ASSERT_EQ(st_insert(st, "key", &value1), 0);
  ASSERT_EQ(*(int *)st_set(st, "key", &value2), value1);
  ASSERT_EQ(*(int *)st_get(st, "key"), value2);

  st_free(st);
}

TEST(SymbolTableTest, GetWithDefault) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);

  int default_value = 42;
  ASSERT_EQ(*(int *)st_get(st, "key", &default_value), default_value);

  st_free(st);
}

TEST(SymbolTableTest, RemoveWithDefault) {
  st_t st = st_new();
  ASSERT_NE(st, nullptr);

  int default_value = 42;
  ASSERT_EQ(*(int *)st_remove(st, "key", &default_value), default_value);

  st_free(st);
}
