#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <unordered_set>
using namespace std;

template <class T>
class ObjectPool {
public:
	T* Allocate() {
		if (_liberated.empty()) {
			T* pointer = new T();
			_allocated.insert(pointer);
			return pointer;
		}

		T* pointer = _liberated.front();
		_liberated.pop();
		_allocated.insert(pointer);
		return pointer;
	}
  T* TryAllocate() {
	  if (_liberated.empty())
		  return nullptr;

	  T* pointer = _liberated.front();
	  _liberated.pop();
	  _allocated.insert(pointer);
	  return pointer;
  }

  void Deallocate(T* object) {
	  if (_allocated.count(object) == 0)
		  throw invalid_argument("");

	  _allocated.erase(object);
	  _liberated.push(object);
  }

  ~ObjectPool() {
	  for (auto p : _allocated) {
		  delete p;
	  }

	  while( !_liberated.empty()) {
		  auto p = _liberated.front();
		  _liberated.pop();
		  delete p;
	  }
  }

private:
	unordered_set<T*> _allocated;
	queue<T*> _liberated;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  cout << 10e3;
  return 0;
}
