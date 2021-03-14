#include "ini.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

Ini::Section& Ini::Document::AddSection(std::string name) {
	return sections[name];
}


const Ini::Section& Ini::Document::GetSection(const std::string& name) const {
	return sections.at(name);
}

size_t Ini::Document::SectionCount() const {
	return sections.size();
}

Ini::Document Ini::Load(std::istream& input) {
	string line;
	Document doc;

	while (input.peek() == ' ' && input.get()) {
	}

	while (getline(input, line)) {
		if (line[0] == '[') {
			size_t pos_1 = line.find(']');
			Section* section = &doc.AddSection(line.substr(1, pos_1 - 1));

			while (input.peek() != '[' && getline(input, line) && line != "") {
				size_t pos_2 = line.find('=');
				section->insert({ line.substr(0, pos_2),
					line.substr(pos_2 + 1) });
			}
		}

		while (input.peek() == ' ' && input.get()) {
		}
	}

	return doc;
}