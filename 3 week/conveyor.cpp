#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;


struct Email {
    string from;
    string to;
    string body;

    Email(string  f, string  t, string  b) : from(std::move(f)), to(std::move(t)), body(std::move(b)) {}
};


class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        // С‚РѕР»СЊРєРѕ РїРµСЂРІРѕРјСѓ worker-Сѓ РІ РїР°Р№РїР»Р°Р№РЅРµ РЅСѓР¶РЅРѕ СЌС‚Рѕ РёРјРїР»РµРјРµРЅС‚РёСЂРѕРІР°С‚СЊ
        throw logic_error("Unimplemented");
    }

protected:
    // СЂРµР°Р»РёР·Р°С†РёРё РґРѕР»Р¶РЅС‹ РІС‹Р·С‹РІР°С‚СЊ PassOn, С‡С‚РѕР±С‹ РїРµСЂРµРґР°С‚СЊ РѕР±СЉРµРєС‚ РґР°Р»СЊС€Рµ
    // РїРѕ С†РµРїРѕС‡РєРµ РѕР±СЂР°Р±РѕС‚С‡РёРєРѕРІ
    void PassOn(unique_ptr<Email> email) const {
        next_->Process(move(email));
    }

public:
    void SetNext(unique_ptr<Worker> next) {
        next_ = move(next);
    }
private:
    unique_ptr<Worker> next_;
};


class Reader : public Worker {
public:
    explicit Reader(istream& is) : private_is(is) {}

    void Process(unique_ptr<Email> email) override {
        Run();
    }

    void Run() override {
        while (private_is) {
            string from;
            string to;
            string body;
            string empty_string;
            getline(private_is, from);
            getline(private_is, to);
            getline(private_is, body);
            PassOn(make_unique<Email>(from, to, body));
        }
    }


private:
    istream& private_is;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

public:
    explicit Filter(Function function) : functor(move(function)) {}

    void Process(unique_ptr<Email> email) override {
        if (functor(*email)) {
            PassOn(move(email));
        }
    }
private:
    Function functor;
};


class Copier : public Worker {
public:
    explicit Copier(const string& address) : address_(address) {}

    void Process(unique_ptr<Email> email) override {
        string copy_address = email->to;
         if (copy_address != address_) {
             unique_ptr<Email> tmp(new Email{email->from, address_, email->body});
             PassOn(move(email));
             PassOn(move(tmp));
         }
         else {
             PassOn(move(email));
         }

    }
private:
    string address_;
};


class Sender : public Worker {
public:
    explicit Sender(ostream& out) : os(out) {}

    void Process(unique_ptr<Email> email) override {
        os << email->from << '\n';
        os << email->to << '\n';
        os << email->body << '\n';
    }
private:
    ostream& os;
};


// СЂРµР°Р»РёР·СѓР№С‚Рµ РєР»Р°СЃСЃ
class PipelineBuilder {
public:
    // РґРѕР±Р°РІР»СЏРµС‚ РІ РєР°С‡РµСЃС‚РІРµ РїРµСЂРІРѕРіРѕ РѕР±СЂР°Р±РѕС‚С‡РёРєР° Reader
    explicit PipelineBuilder(istream& in) {
        pipeline.push_back(make_unique<Reader>(in));
    }

    // РґРѕР±Р°РІР»СЏРµС‚ РЅРѕРІС‹Р№ РѕР±СЂР°Р±РѕС‚С‡РёРє Filter
    PipelineBuilder& FilterBy(Filter::Function filter) {
        pipeline.push_back(make_unique<Filter>(filter));
        return *this;
    }

    // РґРѕР±Р°РІР»СЏРµС‚ РЅРѕРІС‹Р№ РѕР±СЂР°Р±РѕС‚С‡РёРє Copier
    PipelineBuilder& CopyTo(string recipient) {
        pipeline.push_back(make_unique<Copier>(recipient));
        return *this;
    }

    // РґРѕР±Р°РІР»СЏРµС‚ РЅРѕРІС‹Р№ РѕР±СЂР°Р±РѕС‚С‡РёРє Sender
    PipelineBuilder& Send(ostream& out) {
        pipeline.push_back(make_unique<Sender>(out));
        return *this;
    }

    // РІРѕР·РІСЂР°С‰Р°РµС‚ РіРѕС‚РѕРІСѓСЋ С†РµРїРѕС‡РєСѓ РѕР±СЂР°Р±РѕС‚С‡РёРєРѕРІ
    unique_ptr<Worker> Build() {
        for (int i = pipeline.size() - 1; i > 0; i--) {
            pipeline[i-1]->SetNext(move(pipeline[i]));
        }
        return move(pipeline[0]);
    }
private:
    vector<unique_ptr<Worker>> pipeline;
};


void TestSanity() {
    string input = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "ralph@example.com\n"
            "erich@example.com\n"
            "I do not make mistakes of that kind\n"
    );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
    });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "erich@example.com\n"
            "richard@example.com\n"
            "Are you sure you pressed the right button?\n"
    );
    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}
