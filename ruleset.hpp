#pragma once
#include <string>
#include <vector>
#include <array>
#include <map>
#include "tokenizer.hpp"
#include "helpers.hpp"
using namespace std;


struct Ruleset {
	struct Rule {
		string name, type;
		vector<string> subrules;
	};

	map<string, Rule> rules;
	Tokenizer tok;

	void showrules() {
		for (const auto& r : rules) {
			printf("%s  (%s)\n", r.second.name.c_str(), r.second.type.c_str());
			for (const auto& sub : r.second.subrules)
				printf("\t> %s\n", sub.c_str());
		}
	}

	void addrule(const string& name, const string& type, const string& subrules) {
		if (rules.count(name) == 1) throw parse_error("duplicate rule definition: " + name);
		rules[name] = { name, type, splitws(subrules) };
		if (rules[name].subrules.size() == 0) throw parse_error("no subrules in rule: " + name);
	}

	void addrules(const vector<array<string, 3>>& rulelist) {
		for (auto& rs : rulelist)
			addrule(rs[0], rs[1], rs[2]);
	}

	int runrule(const string& name) {
		int pos = tok.pos;
		#define gettok()    ( tok.get(), printf("found token: [%s]\n", name.c_str()), 1 )
		#define getrule()   ( printf("found rule: [%s]\n", name.c_str()), 1 )
		#define unget()     ( tok.pos = pos, 0 )
		// #define errtok()    ( throw parse_error(string("error at token: ") + tok.peek()), 0 )
		#define reqerror()  ( throw parse_error(string("rule required: ") + name + ", got [" + tok.peek() + "]"), 0 )
		// built in rules
		if      (name.length() < 2)  throw parse_error(string("bad rule [") + name + "]");
		// rule modifiers
		else if (name[0] == '$')     return tok.peek() == name.substr(1) ? gettok() : 0;  // string literal
		else if (name[0] == '!')     return runrule(name.substr(1)) ? 1 : reqerror();  // run rule or error
		else if (name[0] == '?')     return runrule(name.substr(1)), 1;  // rule optional
		else if (name[0] == '*')     { while (runrule(name.substr(1))) ; return 1; }
		// literal rules
		else if (name == "eol")      return tok.peek() == tok.TOK_EOL ? gettok() : 0;
		else if (name == "eof")      return tok.peek() == tok.TOK_EOF ? 1 : 0;
		else if (name == "endl")     return tok.peek() == tok.TOK_EOL || tok.peek() == tok.TOK_EOF ? gettok() : 0;
		else if (name == "ident")    return isident(tok.peek()) ? gettok() : 0;
		// check rule existance
		else if (!rules.count(name)) throw parse_error("missing rule: " + name);
		// run rules in order
		for (const auto& sr : rules[name].subrules)
			if (!runrule(sr))
				return unget();
		return getrule();


		// run rule by rule type
		// if (rule.type == "request") {
		// 	for (auto& sr : rule.subrules)
		// 		if (!runrule(sr)) 
		// 			return unget();
		// 	return getrule();
		// }
		// else if (rule.type == "require") {
		// 	for (auto& sr : rule.subrules)
		// 		if (!runrule(sr)) 
		// 			return errtok();
		// 	return getrule();
		// }
		// else if (rule.type == "mul") {
		// 	while (true) {
		// 		for (auto& sr : rule.subrules)
		// 			if (runrule(sr)) 
		// 				continue;
		// 		return 1;
		// 	}
		// }


		// if (rule.type == "get") {
		// 	for (auto& sr : rule.subrules)
		// 		runrule2(sr);
		// 	return getrule();
		// }
		// else if (rule.type == "peek") {
		// 	int pos = tok.pos;
		// 	// string rname;
		// 	try {
		// 		for (auto& sr : rule.subrules)
		// 			// runrule2(rname = sr);
		// 			runrule2(sr);
		// 	}
		// 	catch (parse_error& e) {
		// 		// printf("error peeking rule [%s]: %s\n", rname.c_str(), e.what());
		// 		printf("error peeking rule [%s]: %s\n", rule.name.c_str(), e.what());
		// 		tok.pos = pos;
		// 		return 0;
		// 	}
		// 	printf("peek rule [%s] ok\n", rule.name.c_str());
		// 	tok.pos = pos;
		// 	return 1;
		// }
		// else if (rule.type == "mul") {
		// 	int found = 1;
		// 	while (found) {
		// 		found = 0;
		// 		for (auto& sr : rule.subrules)
		// 			if (runrule2(sr)) found = 1;
		// 	}
		// 	return 1;
		// }
		// else if (rule.type == "any") {
		// 	for (auto& sr : rule.subrules)
		// 		if (runrule2(sr)) return 1;
		// 	return 0;
		// }
		// else if (rule.type == "if") {
		// 	if      (rule.subrules.size() != 2) throw parse_error("rule-type if requires 2 subrules, found " + to_string(rule.subrules.size()));
		// 	else if (runrule2(rule.subrules[0])) return runrule2(rule.subrules[1]), 1;
		// 	else    return 0;
		// }
		// rule type not found
		throw parse_error("unknown rule-type: " + rules[name].type);
	}
};