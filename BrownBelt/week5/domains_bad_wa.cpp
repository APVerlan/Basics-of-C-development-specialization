#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"

using namespace std;


bool IsSubdomain(string_view subdomain, string_view domain) {
	if (subdomain.size() == domain.size())
		return subdomain == domain;

  int i = 0;
  int j = 0;
  while (i <= subdomain.size() - 1 && j <= domain.size() - 1) {
    if (subdomain[i++] != domain[j++]) {
      return false;
    }
  }
  return (i > subdomain.size() - 1 && domain[j] == '.')
      || (j > domain.size() - 1 && subdomain[i] == '.');
}


vector<string> ReadDomains() {
  size_t count;
  cin >> count;
  cin.get();

  vector<string> domains;
  for (size_t i = 0; i < count; ++i) {
    string domain;
    getline(cin, domain);
    domains.push_back(domain);
  }
  return domains;
}

vector<bool> GetAnswer(vector<string>& banned_domains, vector<string>& domains_to_check) {
	vector<bool> ans(domains_to_check.size(), true);

	for (string& domain : banned_domains) {
		reverse(begin(domain), end(domain));
	}
	sort(begin(banned_domains), end(banned_domains));

	size_t insert_pos = 0;
	for (string& domain : banned_domains) {
		if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
			swap(banned_domains[insert_pos++], domain);
		}
	}
	banned_domains.resize(insert_pos);

	for (string& domain : domains_to_check) {
		reverse(begin(domain), end(domain));
	}

	for (int i = 0; i < domains_to_check.size(); i++) {
		if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domains_to_check[i]);
			it != begin(banned_domains) && IsSubdomain(domains_to_check[i], *prev(it))) {
			ans[i] = false;
		}
	}

	return ans;
}

void TetsGetAnswer() {
	vector<string> banned_domains = {"ya.ru", "maps.me", "m.ya.ru", "com"};
	vector<string> domains_to_check = {"ya.ru", "ya.com", "m.maps.me", "moscow.m.ya.ru", "maps.com", "maps.ru", "ya.ya"};

	vector<bool> answer = { false, false, false, false, false, true, true};
	ASSERT_EQUAL(GetAnswer(banned_domains, domains_to_check), answer);

	banned_domains = {"ya.ya", "ya.ru", "ya.com"};
	domains_to_check = { "haya.ya", "teya.ru", "suya.com",
		"ha.ya.ya", "te.ya.ru", "su.ya.com" };
	answer = { true, true, true, false, false, false };
	ASSERT_EQUAL(GetAnswer(banned_domains, domains_to_check), answer);

	banned_domains = { "com" };
	domains_to_check = {"com.ru", "ru.com.en", "ru.com"};
	answer = { true, true, false};
	ASSERT_EQUAL(GetAnswer(banned_domains, domains_to_check), answer);
}

int main() {
  TestRunner tr;
  tr.RunTest(TetsGetAnswer, "GetAnswerTest");

  vector<string> banned_domains = ReadDomains();
  vector<string> domains_to_check = ReadDomains();

  for (bool item : GetAnswer(banned_domains, domains_to_check)) {
	  if (item)
		  cout << "Good\n";
	  else
		  cout << "Bad\n";
  }
  return 0;
}
