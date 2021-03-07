#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <iterator>
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

// Реализуйте этот класс
class Database {
public:
  bool Put(const Record& record) {
		if (data_.count(record.id) == 0) {
			auto it_1 = records_by_user_.insert(make_pair(record.user, nullptr));
			auto it_2 = records_by_karma_.insert(make_pair(record.karma, nullptr));
			auto it_3 = records_by_timestamp_.insert(make_pair(record.timestamp, nullptr));

			data_[record.id] = { record, it_1, it_2, it_3 };

			it_1->second = &data_[record.id].r;
			it_2->second = &data_[record.id].r;
			it_3->second = &data_[record.id].r;

			return true;
		} else return false;
	};

  const Record* GetById(const string& id) const {
		if (!data_.count(id)) 
			return nullptr;
		else return &(data_.at(id).r);
	};

  bool Erase(const string& id) {
	if (data_.count(id)) {
			ValIter item = data_[id];

			records_by_user_.erase(item.iter_user);
			records_by_karma_.erase(item.iter_karma);
			records_by_timestamp_.erase(item.iter_time);

			data_.erase(id);

			return true;
		} else return false;
	};

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
		auto left = records_by_timestamp_.lower_bound(low);
		auto right = records_by_timestamp_.upper_bound(high);

		for (auto it = left; it != right; ++it) {
			bool flag = callback(*(it->second));

			if (!flag)
				return;
		}
	};

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
		auto left = records_by_karma_.lower_bound(low);
		auto right = records_by_karma_.upper_bound(high);

		for (auto it = left; it != right; ++it) {
			bool flag = callback(*(it->second));

			if (!flag)
				return;
		}

	};

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
			auto itr = records_by_user_.equal_range(user);

			for (auto it = itr.first; it!= itr.second; ++it) {
				bool flag = callback(*(it->second));

				if (!flag)
					return;
			}
	};

  struct ValIter {
	  Record r;
	  multimap<string, Record*>::iterator iter_user;
	  multimap<int, Record*>::iterator iter_karma;
	  multimap<int, Record*>::iterator iter_time;
  };

private: 
	unordered_map<string, ValIter> data_;
	multimap<string, Record*> records_by_user_;
	multimap<int, Record*> records_by_karma_;
	multimap<int, Record*> records_by_timestamp_;
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
