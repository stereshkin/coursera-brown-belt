#include "test_runner.h"
#include <limits>
#include <random>
#include <unordered_set>

using namespace std;

using CoordType = int;

struct Point3D {
    CoordType x;
    CoordType y;
    CoordType z;

    bool operator==(const Point3D& other) const {
        return tie(this->x, this->y, this->z) == tie(other.x, other.y, other.z);
    }
};

struct Hasher {
    size_t operator() (const Point3D& point) const {
        hash<int> coord_hasher;
        size_t num = 71;
        auto hash_x = coord_hasher(point.x);
        auto hash_y = coord_hasher(point.y);
        auto hash_z = coord_hasher(point.z);

        return hash_x*num*num + hash_y*num + hash_z;
    }
};

void TestSmoke() {
    vector<Point3D> points = {
            {1, 2, 3},
            {0, 2, 3},
            {1, 0, 3},
            {1, 2, 0},
    };

    unordered_set<Point3D, Hasher> point_set;
    for (const auto& point : points) {
        point_set.insert(point);
    }

    ASSERT_EQUAL(points.size(), point_set.size());
    for (const auto& point : points) {
        ASSERT_EQUAL(point_set.count(point), static_cast<size_t>(1));
    }
}

void TestPurity() {
    Point3D point = {1, 2, 3};
    Hasher hasher;

    auto hash = hasher(point);
    for (size_t t = 0; t < 100; ++t) {
        ASSERT_EQUAL(hasher(point), hash);
    }
};

void TestX() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    bool component_matters = false;
    for (size_t t = 0; t < 1000; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        Point3D other_point = point;
        other_point.x = dist(gen);

        component_matters |= (hasher(point) != hasher(other_point));
    }

    ASSERT(component_matters);
}

void TestY() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    bool component_matters = false;
    for (size_t t = 0; t < 1000; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        Point3D other_point = point;
        other_point.y = dist(gen);

        component_matters |= (hasher(point) != hasher(other_point));
    }

    ASSERT(component_matters);
}

void TestZ() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    bool component_matters = false;
    for (size_t t = 0; t < 1000; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        Point3D other_point = point;
        other_point.z = dist(gen);

        component_matters |= (hasher(point) != hasher(other_point));
    }

    ASSERT(component_matters);
}

void TestDistribution() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    // РІС‹Р±РёСЂР°РµРј С‡РёСЃР»Рѕ Р±Р°РєРµС‚РѕРІ РЅРµ РѕС‡РµРЅСЊ Р±РѕР»СЊС€РёРј РїСЂРѕСЃС‚С‹Рј С‡РёСЃР»РѕРј
    // (unordered_set, unordered_map РёСЃРїРѕР»СЊР·СѓСЋС‚ РїСЂРѕСЃС‚С‹Рµ С‡РёСЃР»Р° Р±Р°РєРµС‚РѕРІ
    // РІ СЂРµР°Р»РёР·Р°С†РёСЏС… GCC Рё Clang, РґР»СЏ РЅРµРїСЂРѕСЃС‚С‹С… С‡РёСЃРµР» Р±Р°РєРµС‚РѕРІ
    // РІРѕР·РЅРёРєР°СЋС‚ СЃР»РѕР¶РЅРѕСЃС‚Рё СЃРѕ СЃС‚Р°РЅРґР°СЂС‚РЅРѕР№ С…РµС€-С„СѓРЅРєС†РёРµР№ РІ СЌС‚РёС… СЂРµР°Р»РёР·Р°С†РёСЏС…)
    const size_t num_buckets = 2053;

    // РјС‹ С…РѕС‚РёРј, С‡С‚РѕР±С‹ С‡РёСЃР»Рѕ С‚РѕС‡РµРє РІ Р±Р°РєРµС‚Р°С… Р±С‹Р»Рѕ ~100'000
    const size_t perfect_bucket_size = 50;
    const size_t num_points = num_buckets * perfect_bucket_size;
    vector<size_t> buckets(num_buckets);
    for (size_t t = 0; t < num_points; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        ++buckets[hasher(point) % num_buckets];
    }

    // РЎС‚Р°С‚РёСЃС‚РёРєР° РџРёСЂСЃРѕРЅР°:
    // https://en.wikipedia.org/wiki/Pearson's_chi-squared_test
    //
    // Р§РёСЃР»РµРЅРЅРѕР№ РјРµСЂРѕР№ СЂР°РІРЅРѕРјРµСЂРЅРѕСЃС‚Рё СЂР°СЃРїСЂРµРґРµР»РµРЅРёСЏ С‚Р°РєР¶Рµ РјРѕР¶РµС‚ РІС‹СЃС‚СѓРїР°С‚СЊ
    // СЌРЅС‚СЂРѕРїРёСЏ, РЅРѕ РґР»СЏ РµРµ РїРѕСЂРѕРіРѕРІ РЅРµС‚ С…РѕСЂРѕС€РµР№ СЃС‚Р°С‚РёСЃС‚РёС‡РµСЃРєРѕР№ РёРЅС‚РµСЂРїСЂРµС‚Р°С†РёРё
    double pearson_stat = 0;
    for (auto bucket_count : buckets) {
        size_t count_diff = bucket_count - perfect_bucket_size;
        pearson_stat +=
                count_diff * count_diff / static_cast<double>(perfect_bucket_size);
    }

    // РїСЂРѕРІРµСЂСЏРµРј СЂР°РІРЅРѕРјРµСЂРЅРѕСЃС‚СЊ СЂР°СЃРїСЂРµРґРµР»РµРЅРёСЏ РјРµС‚РѕРґРѕРј СЃРѕРіР»Р°СЃРёСЏ РџРёСЂСЃРѕРЅР°
    // СЃРѕ СЃС‚Р°С‚РёСЃС‚РёС‡РµСЃРєРѕР№ Р·РЅР°С‡РёРјРѕСЃС‚СЊСЋ 0.95:
    //
    // РµСЃР»Рё РїРѕРґСЃС‚Р°РІРёС‚СЊ РІРјРµСЃС‚Рѕ ++buckets[hasher(point) % num_buckets]
    // РІС‹СЂР°Р¶РµРЅРёРµ ++buckets[dist(gen) % num_buckets], С‚Рѕ СЃ РІРµСЂРѕСЏС‚РЅРѕСЃС‚СЊСЋ 95%
    // ASSERT РЅРёР¶Рµ РѕС‚СЂР°Р±РѕС‚Р°РµС‚ СѓСЃРїРµС€РЅРѕ,
    //
    // С‚.Рє. СЃС‚Р°С‚РёСЃС‚РёРєР° РџРёСЂСЃРѕРЅР° РїСЂРёР±Р»РёР·РёС‚РµР»СЊРЅРѕ СЂР°СЃРїСЂРµРґРµР»РµРЅР° РїРѕ chi^2
    // СЃ С‡РёСЃР»РѕРј СЃС‚РµРїРµРЅРµР№ СЃРІРѕР±РѕРґС‹, СЂР°РІРЅС‹Рј num_buckets - 1,
    // Рё 95 РїСЂРѕС†РµРЅС‚РёР»СЊ СЌС‚РѕРіРѕ СЂР°СЃРїСЂРµРґРµР»РµРЅРёСЏ СЂР°РІРµРЅ:
    // >>> scipy.stats.chi2.ppf(0.95, 2052)
    const double critical_value = 2158.4981036918693;
    ASSERT(pearson_stat < critical_value);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSmoke);
    RUN_TEST(tr, TestPurity);
    RUN_TEST(tr, TestX);
    RUN_TEST(tr, TestY);
    RUN_TEST(tr, TestZ);
    RUN_TEST(tr, TestDistribution);

    return 0;
}
