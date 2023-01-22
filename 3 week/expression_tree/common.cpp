#include "common.h"
#include <memory>

using namespace std;

Val::Val(int value) : value_(value) {};

Val::Val(Val&& another_val) : value_(another_val.value_), to_str(move(another_val.to_str)) {}

int Val::Evaluate() const {
    return value_;
}

std::string Val::ToString() const {
    if (!to_str.empty()) {
        return to_str;
    }
    to_str = to_string(value_);
    return to_str;
}

ExpressionPtr Value(int value) {
    ExpressionPtr ptr = make_unique<Val>(value);
    return ptr;
}

std::string Summa::ToString() const {
    if (!to_str.empty()) {
        return to_str;
    }
    to_str = "(" + left_->ToString() + ")" "+" + "(" + right_->ToString() + ")";
    return to_str;
}

int Summa::Evaluate() const {
    return move(left_->Evaluate() + right_->Evaluate());
}

Summa::Summa(Summa &&another_sum) : left_(move(another_sum.left_)), right_(move(another_sum.right_)), to_str(move(another_sum.to_str)) {}


Summa::Summa(ExpressionPtr left, ExpressionPtr right) : left_(move(left)), right_(move(right)) {};

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<Summa>(move(left), move(right));
}

Prod::Prod(ExpressionPtr left, ExpressionPtr right) : left_(move(left)), right_(move(right)) {};

int Prod::Evaluate() const {
    return move(left_->Evaluate() * right_->Evaluate());
}

std::string Prod::ToString() const {
    if (!to_str.empty()) {
        return to_str;
    }
    to_str = "(" + left_->ToString() + ")" + "*" + "(" + right_->ToString() + ")";
    return to_str;
}

Prod::Prod(Prod &&another_product) : left_(move(another_product.left_)), right_(move(another_product.right_)), to_str(move(another_product.to_str)) {}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<Prod>(move(left), move(right));
}
