#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>
#include <ctime>
#include <set>
#include <sstream>

using namespace std;

enum class ReqType {
	ComputeIncome,
	Earn,
	PayTax
};

struct Date {
public:
	Date() = default;

	Date(int year, int month, int day) : year_(year), month_(month), day_(day)	{}

	time_t AsTimeStamp() const;

	static long long ComputeDaysDiff(Date date_from, Date date_to);

	static size_t GetIndexFromDay(Date date);

private:
	int year_, month_, day_;
};

struct Request {
	time_t from_, to_;
	ReqType type_;
	int val_ = 0, percent = 13;
};

struct BudgetManager {
	BudgetManager() { money_per_day_ = vector<MoneyPerDay>(40000); }

	void Earn(Date from, Date to, double money) {
		const double money_per_day = money /
			(static_cast<double>(Date::ComputeDaysDiff(from, to)) + 1);

		const size_t n = Date::GetIndexFromDay(to);

		for (size_t i = Date::GetIndexFromDay(from); i <= n; ++i)
		{
			money_per_day_[i].earned += money_per_day;
		}
	}

	void Spend(Date from, Date to, double money) {
		const double money_per_day = money /
			(static_cast<double>(Date::ComputeDaysDiff(from, to)) + 1);

		const size_t n = Date::GetIndexFromDay(to);

		for (size_t i = Date::GetIndexFromDay(from); i <= n; ++i)
		{
			money_per_day_[i].spent += money_per_day;
		}
	}

	void PayTax(Date from, Date to, int tax = 13) {

		const size_t n = Date::GetIndexFromDay(to);

		for (size_t i = Date::GetIndexFromDay(from); i <= n; ++i)
		{
			money_per_day_[i].earned *= ((100.0 - tax * 1.0) / 100.0);
		}
	}

	double CompIncome(Date from, Date to) const {
		double result = 0;

		const size_t n = Date::GetIndexFromDay(to);

		for (size_t i = Date::GetIndexFromDay(from); i <= n; ++i)
		{
			const auto& money = money_per_day_[i];

			result += (money.earned - money.spent);
		}

		return result;
	}

	void ProcessRequest(istream& stream_i = cin, ostream& stream_o = cout);

private:
	struct MoneyPerDay {
		double earned;
		double spent;
	};

	vector<MoneyPerDay> money_per_day_;
};

time_t Date::AsTimeStamp() const {
	std::tm t;
	t.tm_sec = 0;
	t.tm_min = 0;
	t.tm_hour = 0;
	t.tm_mday = day_;
	t.tm_mon = month_ - 1;
	t.tm_year = year_ - 1900;
	t.tm_isdst = 0;
	return mktime(&t);
}


long long Date::ComputeDaysDiff(Date date_from, Date date_to) {
	const time_t timestamp_to = date_to.AsTimeStamp();
	const time_t timestamp_from = date_from.AsTimeStamp();
	const auto SECONDS_IN_DAY = 60 * 60 * 24;

	return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}


size_t Date::GetIndexFromDay(Date date) {
	return static_cast<size_t>(ComputeDaysDiff(Date(2000, 1, 1), date));
}

Date ParseDate(string& date) {
	istringstream ss(date);

	size_t day_,
		month_,
		year_;

	ss >> year_;
	ss.get();
	ss >> month_;
	ss.get();
	ss >> day_;

	return Date(year_, month_, day_);
}

void BudgetManager::ProcessRequest(istream& stream_i, ostream& stream_o) {
	cout.precision(25);
	//stream_i.tie(nullptr);
	//std::ios_base::sync_with_stdio(false);

	string type, date_from, date_to;
	stream_i >> type >> date_from >> date_to;

	Date from = ParseDate(date_from),
		to = ParseDate(date_to);


	if (type == "ComputeIncome") { 
		stream_o << CompIncome(from, to) << endl;
	}
	else if (type == "Earn") { 
		int val;
		stream_i >> val;
		Earn(from, to, val);
	}
	else if (type == "PayTax") {
		int percent;
		stream_i >> percent;
		PayTax(from, to, percent); 
	}
	else if (type == "Spend") {
		int val;
		stream_i >> val;
		Spend(from, to, val);
	}
}

int main() {
	BudgetManager bm;

	int n;
	cin >> n;

	for (size_t i = 0; i < n; i++) {
		bm.ProcessRequest();
	}

	return 0;
}