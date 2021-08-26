#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t
#include <algorithm>
#include <iostream>

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
  T* ptr_;
public:
  UniquePtr() { ptr_ = nullptr; }
  UniquePtr(T* ptr) : ptr_(ptr) {}
  UniquePtr(UniquePtr&& other) noexcept  { 
	  ptr_ = other.ptr_; 
	  other.ptr_ = nullptr;
  }

  UniquePtr& operator=(nullptr_t) { 
	delete ptr_; 
	ptr_ = nullptr;
	return *this;
  }

  UniquePtr& operator=(UniquePtr&& other) noexcept {
	delete ptr_;
	ptr_ = other.ptr_; 
	other.ptr_ = nullptr;
	return *this;
  }

  ~UniquePtr() {
	delete ptr_;
  }


  UniquePtr& operator=(const UniquePtr&) = delete;

  UniquePtr(const UniquePtr&) = delete;
  T& operator*() const { return *ptr_; }

  T* operator->() const { return ptr_; }

  T* Release() {
	T* copy = ptr_;
	ptr_ = nullptr;
	return copy;
  }

  void Reset(T* ptr) {
	delete ptr_;
	ptr_ = ptr;
  }

  void Swap(UniquePtr& other) {//std::swap(other.ptr_, ptr_); 
  
	  T* tmp = std::move(ptr_);
	  ptr_ = std::move(other.ptr_);
	  other.ptr_ = std::move(tmp);
  }

  T* Get() const { return ptr_; }
};


struct Item {
  static int counter;
  int value;
  Item(int v = 0): value(v) {
    ++counter;
  }
  Item(const Item& other): value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;


void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

void TestSwap() {
	int* p1 = new int(10);
	int* p2 = new int(20);
	UniquePtr<int> u_ptr1(p1), u_ptr2(p2);
	
	ASSERT_EQUAL(*u_ptr1, 10);
	ASSERT_EQUAL(*u_ptr2, 20);
	ASSERT_EQUAL(u_ptr1.Get(), p1);
	ASSERT_EQUAL(u_ptr2.Get(), p2);

	u_ptr1.Swap(u_ptr2);
	ASSERT_EQUAL(u_ptr1.Get(), p2);
	ASSERT_EQUAL(u_ptr2.Get(), p1);
}

void TestNull() {
	UniquePtr<int> u_ptr;
	ASSERT(u_ptr.Get() == nullptr);

	int* p1 = new int(10);
	UniquePtr<int> u_ptr1(p1);

	u_ptr1 = nullptr;
	ASSERT(u_ptr1.Get() == nullptr);
}

void TestReset() {
	int* p1 = new int(10);
	int* p2 = new int(20);
	UniquePtr<int> u_ptr1(p1);

	ASSERT_EQUAL(*u_ptr1, 10);

	u_ptr1.Reset(p2);

	ASSERT_EQUAL(*u_ptr1, 20);
}

void TestMove() {
	int* p1 = new int(10);
	int* p2 = new int(20);
	UniquePtr<int> u_ptr1(p1), u_ptr2(p2);

	u_ptr1 = move(u_ptr2);
	ASSERT(u_ptr2.Get() == nullptr);
	u_ptr2 = UniquePtr<int>(p1);

	u_ptr1 = UniquePtr<int>(move(u_ptr2));
	ASSERT_EQUAL(*u_ptr1, 10)
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
  RUN_TEST(tr, TestSwap);
  RUN_TEST(tr, TestNull);
  RUN_TEST(tr, TestReset);
}
