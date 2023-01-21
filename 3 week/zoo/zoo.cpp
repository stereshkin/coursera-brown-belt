#include "animals.h"
#include "test_runner.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

using namespace std;

using Zoo = vector<unique_ptr<Animal>>;

// Р­С‚Р° С„СѓРЅРєС†РёСЏ РїРѕР»СѓС‡Р°РµС‚ РЅР° РІС…РѕРґ РїРѕС‚РѕРє РІРІРѕРґР° Рё С‡РёС‚Р°РµС‚ РёР· РЅРµРіРѕ РѕРїРёСЃР°РЅРёРµ Р·РІРµСЂРµР№.
// Р•СЃР»Рё РѕС‡РµСЂРµРґРЅРѕРµ СЃР»РѕРІРѕ СЌС‚РѕРіРѕ С‚РµРєСЃС‚Р° - Tiger, Wolf РёР»Рё Fox, С„СѓРЅРєС†РёСЏ РґРѕР»Р¶РЅР° РїРѕРјРµСЃС‚РёС‚СЊ СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓСЋС‰РµРіРѕ Р·РІРµСЂСЏ РІ Р·РѕРѕРїР°СЂРє.
// Р’ РїСЂРѕС‚РёРІРЅРѕРј СЃР»СѓС‡Р°Рµ РѕРЅР° РґРѕР»Р¶РЅР° РїСЂРµРєСЂР°С‚РёС‚СЊ С‡С‚РµРЅРёРµ Рё СЃРіРµРЅРµСЂРёСЂРѕРІР°С‚СЊ РёСЃРєР»СЋС‡РµРЅРёРµ runtime_error.
Zoo CreateZoo(istream& in) {
    Zoo zoo;
    string word;
    while (in >> word) {
        if (word == "Tiger") {
            unique_ptr<Animal> t = make_unique<Tiger>();
            zoo.emplace_back(move(t));
        } else if (word == "Wolf") {
            unique_ptr<Animal> w = make_unique<Wolf>();
            zoo.emplace_back(move(w));
        } else if (word == "Fox") {
            unique_ptr<Animal> f = make_unique<Fox>();
            zoo.emplace_back(move(f));
        } else {
            throw runtime_error("Unknown animal!");
        }
    }
    return zoo;
}

// Р­С‚Р° С„СѓРЅРєС†РёСЏ РґРѕР»Р¶РЅР° РїРµСЂРµР±СЂР°С‚СЊ РІСЃРµС… Р·РІРµСЂРµР№ РІ Р·РѕРѕРїР°СЂРєРµ РІ РїРѕСЂСЏРґРєРµ РёС… СЃРѕР·РґР°РЅРёСЏ
// Рё Р·Р°РїРёСЃР°С‚СЊ РІ РІС‹С…РѕРґРЅРѕР№ РїРѕС‚РѕРє РґР»СЏ РєР°Р¶РґРѕРіРѕ РёР· РЅРёС… СЂРµР·СѓР»СЊС‚Р°С‚ СЂР°Р±РѕС‚С‹ РІРёСЂС‚СѓР°Р»СЊРЅРѕР№ С„СѓРЅРєС†РёРё voice.
// Р Р°Р·РґРµР»СЏР№С‚Рµ РіРѕР»РѕСЃР° СЂР°Р·РЅС‹С… Р·РІРµСЂРµР№ СЃРёРјРІРѕР»РѕРј РїРµСЂРµРІРѕРґР° СЃС‚СЂРѕРєРё '\n'.
void Process(const Zoo& zoo, ostream& out) {
    for (const auto& animal : zoo) {
        out << animal->Voice()<< "\n";
    }
}

void TestZoo() {
    istringstream input("Tiger Wolf Fox Tiger");
    ostringstream output;
    Process(CreateZoo(input), output);

    const string expected =
            "Rrrr\n"
            "Wooo\n"
            "Tyaf\n"
            "Rrrr\n";

    ASSERT_EQUAL(output.str(), expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestZoo);
}
