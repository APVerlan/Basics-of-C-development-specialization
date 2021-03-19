#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
	void PassOn(unique_ptr<Email> email) const {
		if(_next)
			_next->Process(move(email));
	};

public:
	void SetNext(unique_ptr<Worker> next) {
		if (!_next)
			_next = move(next);
		else
			_next->SetNext(move(next));
	};

private:
	unique_ptr<Worker> _next;
};


class Reader : public Worker {
public:
	Reader(istream& input) : _input(input) {}
	void Run() {
		while (_input.peek() != EOF) {
			string from, to, body;
			getline(_input, from);
			getline(_input, to);
			getline(_input, body);

			Email e = { from, to, body };
			PassOn(make_unique<Email>(e));
		}
	}

	void Process(unique_ptr<Email> email) {};

private:
	istream& _input;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;
  Filter(Function f) : _predicate(f) {}
  void Process(unique_ptr<Email> email) {
	  if (_predicate(*email))
		  PassOn(move(email));
  }
private:
	Function _predicate;
};


class Copier : public Worker {
public:
	Copier(const string& to) : _to(to) {}
	void Process(unique_ptr<Email> email) {
		if (email->to == _to)
			PassOn(move(email));
		else {
			Email copy = { email->from, _to, email->body };
			PassOn(move(email));
			PassOn(make_unique<Email>(copy));
		}
	}
private:
	string _to;
};


class Sender : public Worker {
public:
	Sender(ostream& output) : _output(output) {};
	void Process(unique_ptr<Email> email) {
		_output << email->from << "\n"
			<< email->to << "\n"
			<< email->body << '\n';
		PassOn(move(email));
	};

private:
	ostream& _output;
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
	explicit PipelineBuilder(istream& in) {
		//Reader reader(in);
		_input_worker = move(make_unique<Reader>(in));
	};

  // добавляет новый обработчик Filter
	PipelineBuilder& FilterBy(Filter::Function filter) {
		//Filter ftr(filter);
		_input_worker->SetNext(make_unique<Filter>(filter));

		return *this;
	};

  // добавляет новый обработчик Copier
	PipelineBuilder& CopyTo(string recipient) {
		//Copier copier(recipient);
		_input_worker->SetNext(make_unique<Copier>(recipient));

		return *this;
	};

  // добавляет новый обработчик Sender
	PipelineBuilder& Send(ostream& out) {
		//Sender sender(out);
		_input_worker->SetNext(make_unique<Sender>(out));

		return *this;
	};

  // возвращает готовую цепочку обработчиков
	unique_ptr<Worker> Build() {
		return move(_input_worker);
	};

private:
	unique_ptr<Worker> _input_worker;
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
