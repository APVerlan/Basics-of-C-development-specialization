#pragma once

#include <mutex>
#include <future>

using namespace std;

template <typename T>
class Synchronized {
public:
	explicit Synchronized(T initial = T()) : value(move(initial)) {};

	struct Access {
	public:

		Access(T& value, std::mutex& mutex) : ref_to_value(value), guard(mutex) {}
		T& ref_to_value;

	private:
		lock_guard<mutex> guard;
	};

	Access GetAccess() { return {value, m}; };
private:
	T value;
	mutex m;
};