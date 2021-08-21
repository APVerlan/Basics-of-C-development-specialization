#include <iostream>
#include <vector>
#include <algorithm>

void PrintStats(std::vector<Person> persons) {
	std::cout << "Median age = " << ComputeMedianAge(persons.begin(), persons.end()) 
		<< std::endl;

	auto it_g = std::partition(persons.begin(), persons.end(),
		[](Person p) {return p.gender == Gender::FEMALE; });
	std::cout << "Median age for females = " << ComputeMedianAge(persons.begin(), it_g)
		<< std::endl;
	std::cout << "Median age for males = " << ComputeMedianAge(it_g, persons.end())
		<< std::endl;

	auto it_em = std::partition(persons.begin(), it_g,
		[](Person p) {return p.is_employed; });
	std::cout << "Median age for employed females = " << ComputeMedianAge(persons.begin(), it_em)
		<< std::endl;
	std::cout << "Median age for unemployed females = " << ComputeMedianAge(it_em, it_g)
		<< std::endl;

	it_em = std::partition(it_g, persons.end(),
		[](Person p) {return p.is_employed; });
	std::cout << "Median age for employed males = " << ComputeMedianAge(it_g, it_em)
		<< std::endl;
	std::cout << "Median age for unemployed males = " << ComputeMedianAge(it_em, persons.end())
		<< std::endl;
}