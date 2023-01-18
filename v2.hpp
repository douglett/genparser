#include <string>
#include <vector>
#include <map>
#include "helpers.hpp"
using namespace std;


struct Rule {
	string name, type;
	vector<string> subrules;
};

map<string, Rule> rules;

void showrules() {
	for (const auto& r : rules) {
		printf("%s  (%s)\n", r.second.name.c_str(), r.second.type.c_str());
		for (const auto& sub : r.second.subrules)
			printf("\t> %s\n", sub.c_str());
	}
}

void addrule(const string& name, const string& type, const string& subrules) {
	rules[name] = { name, type, splitws(subrules) };
}

void runrule(const string& name) {
	
}

void setup() {
	addrule("prog", "mul", "progln");
	addrule("progln", "any", "function emptyline");
	addrule("emptyline", "get", "eol");
	addrule("function", "if", "function_start function_main");
	addrule("function_start", "get", "$function");
	addrule("function_main", "get", "$function $( $) eol function_body $end $function");
	addrule("function_body", "get", "eol");

	showrules();
}