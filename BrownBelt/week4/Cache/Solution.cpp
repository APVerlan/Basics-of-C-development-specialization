#include "Common.h"

#include <future>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <deque>

using namespace std;

class LruCache : public ICache {
public:
	struct RateBook {
		size_t rating;
		string key;
	};

  LruCache( shared_ptr<IBooksUnpacker> books_unpacker, 
	  const Settings& settings) : settings_(settings), unpacker_(books_unpacker) {}

  BookPtr GetBook(const string& book_name) override {
	lock_guard g(m_);
	
	if (books_.count(book_name))
		return books_[book_name];
	else {
		BookPtr ptbook = unpacker_->UnpackBook(book_name);

		if (ptbook->GetContent().size() > settings_.max_memory)
			return ptbook;
		

		__AddBook(ptbook);
		__UpdateCache();

		return ptbook;
	}
  }

private:
	const Settings settings_;
	size_t size_ = 0;
	shared_ptr<IBooksUnpacker> unpacker_;
	mutex m_ = mutex();

	size_t counter_ = 0;
	set<RateBook> rated_books_;
	unordered_map<string, BookPtr> books_;

	void __UpdateCache() {
		while (size_ > settings_.max_memory) {
			string delete_key = rated_books_.begin()->key;

			rated_books_.erase(rated_books_.begin());

			size_ -= books_[delete_key]->GetContent().size();
			books_.erase(delete_key);
		}
	}

	void __AddBook(BookPtr ptbook) {
		/*for (auto rbook : rated_books_) {
			rbook.rating++;
		}*/

		size_ += ptbook->GetContent().size();

		auto iter_m = books_.insert({ ptbook->GetName(), ptbook }).first;
		auto iter_s = rated_books_.insert({ counter_++, ptbook->GetName() }).first;
	}

};

bool operator<(const LruCache::RateBook& lhs, const LruCache::RateBook& rhs) { return lhs.rating < rhs.rating; } // это специально :)

unique_ptr<ICache> MakeCache( shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings) {
	return make_unique<LruCache>(books_unpacker, settings);
}
