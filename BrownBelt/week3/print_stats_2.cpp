#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <string_view>
#include <numeric>
#include <unordered_map>
#include <set>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
  IteratorRange(Iterator begin, Iterator end)
    : first(begin)
    , last(end)
  {
  }

  Iterator begin() const {
    return first;
  }

  Iterator end() const {
    return last;
  }

private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

class PeopleDataBase {
public:
	PeopleDataBase() = default;

	struct Person {
		string name;
		int age, income;
		bool is_male;
	};

	void ReadPeople(istream& input) {
		int count;
		input >> count;

		_sorted_data = vector<Person>(count);
		for (Person& p : _sorted_data) {
			char gender;
			input >> p.name >> p.age >> p.income >> gender;
			p.is_male = gender == 'M';
			
			if (p.is_male)
				_names_man[p.name]++;
			else _names_woman[p.name]++;
		}

		sort(begin(_sorted_data), end(_sorted_data), [](const Person & lhs, const Person & rhs) {
			return lhs.age < rhs.age;
			});

		__BuildStatsData();
	}

	const vector<Person>& GetData() const { return _sorted_data; }

	unsigned short Adults(unsigned short age) const {
		auto adult_begin = lower_bound(
			begin(_sorted_data),
			end(_sorted_data),
			age,
			[](const Person & lhs, int age) {
				return lhs.age < age;
			});

		return std::distance(adult_begin, end(_sorted_data));
	}

	size_t TopIncome(size_t num) const {
		return _stats.incomes[num - 1];
	}

	string GetMostPopularName(char gender) {
		if (gender == 'M')
			return _stats.most_popular_male_name.value();
		else
			return _stats.most_popular_female_name.value();
	}

private:
	struct StatsData {
		std::optional<string> most_popular_male_name;
		std::optional<string> most_popular_female_name;
		vector<size_t> incomes;
	}; 

	void __BuildStatsData() {
		_stats.most_popular_female_name = __MostPopularName(_names_woman);
		_stats.most_popular_male_name = __MostPopularName(_names_man);

		vector<Person> copy = _sorted_data;
		sort(copy.begin(), copy.end(), [](const Person & lhs, const Person & rhs) {
			return lhs.income > rhs.income;
			});

		auto& wealth = _stats.incomes;
		wealth.resize(copy.size());
		if (!copy.empty()) {
			wealth[0] = copy[0].income;
			for (size_t i = 1; i < copy.size(); ++i) {
				wealth[i] = wealth[i - 1] + copy[i].income;
			}
		}
	}

	string __MostPopularName(unordered_map<string, size_t> names) {
		if (names.empty())
			return "";

		pair<string, size_t> most_popular = *names.begin();

		for (auto p : names) {
			if (p.second > most_popular.second)
				most_popular = p;
			else if (p.second == most_popular.second && p.first < most_popular.first) {
				most_popular = p;
			}
		}

		return most_popular.first;
	}


	StatsData _stats;
	vector<Person> _sorted_data;
	unordered_map<string, size_t> _names_man, _names_woman;
	
};

int main() {
	PeopleDataBase db;
	db.ReadPeople(cin);

	for (string command; cin >> command; ) {
		if (command == "AGE") {
			int adult_age;
			cin >> adult_age;

			cout << "There are " << db.Adults(adult_age)
				<< " adult people for maturity age " << adult_age << '\n';
		}
		else if (command == "WEALTHY") {
			int count;
			cin >> count;

			
			cout << "Top-" << count << " people have total income " << db.TopIncome(count) << '\n';
		}
		else if (command == "POPULAR_NAME") {
			char gender;
			cin >> gender;

			string name = static_cast<string>(db.GetMostPopularName(gender));

			if (name == "") {
				cout << "No people of gender " << gender << '\n';
			} else {
				cout << "Most popular name among people of gender " << gender << " is "
					<< name << '\n';
			}
		}
	}
}
