#include "test_runner.h"

#include <forward_list>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
public:
  using BucketList = forward_list<Type>;

public:
  explicit HashSet(
      size_t num_buckets,
      const Hasher& hasher = {}
  ) {
      buckets.reserve(num_buckets);
      for (int i = 0; i < num_buckets; i++) {
          BucketList bl;
          buckets.push_back(bl);
      }
      actual_hasher = hasher;
  }

  void Add(const Type& value) {
      auto ind = actual_hasher(value);
      if (ind >= buckets.size()) {
          buckets.resize(ind+1);
      }
      if (!Has(value)) {
          buckets[ind].push_front(value);
      }


  }
  bool Has(const Type& value) const {
      auto ind = actual_hasher(value);
      if (ind > buckets.size()-1) {
          return false;
      }
      return (find(begin(buckets[ind]), end(buckets[ind]), value)) != end(buckets[ind]);
  }
  void Erase(const Type& value) {
      auto ind = actual_hasher(value);
      BucketList bucket_list = buckets[ind];
      buckets[ind].remove(value);

  }
  const BucketList& GetBucket(const Type& value) const {
      return buckets[actual_hasher(value)];
  }
private:
    vector<BucketList> buckets;
    Hasher actual_hasher;
};

struct IntHasher {
  size_t operator()(int value) const {
    // Р­С‚Рѕ СЂРµР°Р»СЊРЅР°СЏ С…РµС€-С„СѓРЅРєС†РёСЏ РёР· libc++, libstdc++.
    // Р§С‚РѕР±С‹ РѕРЅР° СЂР°Р±РѕС‚Р°Р»Р° С…РѕСЂРѕС€Рѕ, std::unordered_map
    // РёСЃРїРѕР»СЊР·СѓРµС‚ РїСЂРѕСЃС‚С‹Рµ С‡РёСЃР»Р° РґР»СЏ С‡РёСЃР»Р° Р±Р°РєРµС‚РѕРІ
    return value;
  }
};

struct TestValue {
  int value;

  bool operator==(TestValue other) const {
    return value / 2 == other.value / 2;
  }
};

struct TestValueHasher {
  size_t operator()(TestValue value) const {
    return value.value / 2;
  }
};

void TestSmoke() {
  HashSet<int, IntHasher> hash_set(2);
  hash_set.Add(3);
  hash_set.Add(4);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Erase(3);

  ASSERT(!hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Add(3);
  hash_set.Add(5);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(hash_set.Has(5));
}

void TestEmpty() {
  HashSet<int, IntHasher> hash_set(10);
  for (int value = 0; value < 10000; ++value) {
    ASSERT(!hash_set.Has(value));
  }
}

void TestIdempotency() {
  HashSet<int, IntHasher> hash_set(10);
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
  HashSet<TestValue, TestValueHasher> hash_set(10);
  hash_set.Add(TestValue{2});
  hash_set.Add(TestValue{3});

  ASSERT(hash_set.Has(TestValue{2}));
  ASSERT(hash_set.Has(TestValue{3}));

  const auto& bucket = hash_set.GetBucket(TestValue{2});
  const auto& three_bucket = hash_set.GetBucket(TestValue{3});
  ASSERT_EQUAL(&bucket, &three_bucket);

  ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
  ASSERT_EQUAL(2, bucket.front().value);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSmoke);
  RUN_TEST(tr, TestEmpty);
  RUN_TEST(tr, TestIdempotency);
  RUN_TEST(tr, TestEquivalence);
  return 0;
}
