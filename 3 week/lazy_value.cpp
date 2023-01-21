#include "test_runner.h"

#include <functional>
#include <string>
using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(std::function<T()> init) {
        get_times = 0;
        function = init;
    }

    bool HasValue() const {
        return get_times >= 1;
    }
    const T& Get() const {
        get_times += 1;
        if (get_times == 1) {
            auto obj = function();
            object = obj;
        }
        return object;

    }

private:
    mutable int get_times;
    std::function<T()> function;
    mutable T object;

};

void UseExample() {
    const string big_string = "Giant amounts of memory";

    LazyValue<string> lazy_string([&big_string] { return big_string; });

    ASSERT(!lazy_string.HasValue());
    ASSERT_EQUAL(lazy_string.Get(), big_string);
    ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    ASSERT(!called);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, UseExample);
    RUN_TEST(tr, TestInitializerIsntCalled);
    return 0;
}
