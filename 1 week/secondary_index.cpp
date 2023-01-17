#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

// Р РµР°Р»РёР·СѓР№С‚Рµ СЌС‚РѕС‚ РєР»Р°СЃСЃ
class Database {
public:
    bool Put(const Record& record) {
        string id = record.id;
        if (storage.find(id) != end(storage)) {
            return false;
        }
        storage[id] = record;
        timestamps.insert({record.timestamp, record});
        karmas.insert({record.karma, record});
        users.insert({record.user, record});

        return true;
    }
    const Record* GetById(const string& id) const {
        auto itr = storage.find(id);
        if (itr == end(storage)) {
            return nullptr;
        }
        return &(itr->second);
    }
    bool Erase(const string& id) {
        if (storage.find(id) != end(storage)) {
            storage.erase(id);
            return true;
        }
        return false;
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        auto itr = timestamps.lower_bound(low);
        while (itr !=end(timestamps) and itr->first <= high) {
            if (!callback(itr->second)) {
                return;
            }
            itr++;
        }
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        auto itr = karmas.find(low);
        while (itr != end(karmas) and itr->first <= high) {
            if (!callback(itr->second)) {
                return;
            }
            itr++;
        }
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        auto itr = users.find(user);
        while (itr != end(users) and itr->first == user) {
            if (!callback(itr->second)) {
                return;
            }
            itr++;
        }
    }
private:
    unordered_map<string, Record> storage;
    multimap<int, Record> timestamps;
    multimap<int, Record> karmas;
    multimap<string, Record> users;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
