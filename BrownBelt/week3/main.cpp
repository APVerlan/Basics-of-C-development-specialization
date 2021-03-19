#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

class BinaryOperation : public Expression {
public:
	BinaryOperation(char op, ExpressionPtr left,
		ExpressionPtr right) : _operation(op), _left(move(left)),
		_right(move(right)) {}

	int Evaluate() const {
		int left = _left->Evaluate();
		int right = _right->Evaluate();

		if (_operation == '+')
			return left + right;
		else if (_operation == '*')
			return left * right;
	}

	std::string ToString() const {
		std::string left = _left->ToString();
		std::string right = _right->ToString();

		if (_operation == '+')
			return "(" + left + ")" + '+' + "(" + right + ")";
		else if (_operation == '*')
			return "(" + left + ")" + '*' + "(" + right + ")";
	}
private:
	char _operation;
	ExpressionPtr _left, _right;
};

class Digit : public Expression {
public:
	Digit(int val) : _val(val) {}

	std::string ToString() const { return std::to_string(_val); }
	int Evaluate() const { return _val; }

private:
	int _val;
};

ExpressionPtr Value(int value) {
	return make_unique<Digit>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
	return make_unique<BinaryOperation>(
		BinaryOperation('+', move(left), move(right)) );
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
	return make_unique<BinaryOperation>(
		BinaryOperation('*', move(left), move(right)));
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}