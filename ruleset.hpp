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

	int runrule(const string& name, Tokenizer& tok) {
		int pos = tok.pos;
		#define gettok()  (tok.get(), printf("found token: [%s]\n", name.c_str()))
		#define getrule() (printf("found rule: [%s]\n", name.c_str()), 1)
		#define unget()   (tok.pos = pos, 0)
		// built in rules
		// special case - literal rule
		if      (name.length() >= 2 && name[0] == '$') return tok.peek() == name.substr(1) ? (gettok(), 1) : 0;
		else if (name == "eol")  return tok.peek() == tok.TOK_EOL ? (gettok(), 1) : 0;
		else if (name == "eof")  return tok.peek() == tok.TOK_EOF ? 1 : 0;
		else if (name == "endl") return runrule("eol", tok) || runrule("eof", tok);
		// user defined rules
		// check rule existance
		if (rules.count(name) == 0) throw parse_error("missing rule: " + name);
		const auto& rule = rules[name];
		// run rule by rule type
		if (rule.type == "get") {
			for (auto& sr : rule.subrules)
				if (!runrule(sr, tok)) return unget();
			return getrule();
		}
		else if (rule.type == "any") {
			for (auto& sr : rule.subrules)
				if (runrule(sr, tok)) return getrule();
			return unget();
		}
		else if (rule.type == "mul") {
			int found = 1;
			while (found) {
				found = 0;
				for (auto& sr : rule.subrules)
					if (runrule(sr, tok)) found = 1;
			}
			return getrule();
		}
		else if (rule.type == "if") {
			if      (rule.subrules.size() != 2) throw parse_error("rule-type if requires 2 subrules, found " + to_string(rule.subrules.size()));
			else if (runrule(rule.subrules[0], tok)) return unget(), runrule(rule.subrules[1], tok);
			else    return unget();
		}
		throw parse_error("unknown rule-type: " + rule.type);
	}

	// int tokget() {
	// 	tok.get();
	// 	printf("found token: [%s]\n", name.c_str());
	// 	return 1;
	// }

	// int tokerr() {
	// 	throw parse_error("error");
	// }

	int runrule2(const string& name) {
		#define gettok2()  (tok.get(), printf("found token: [%s]\n", name.c_str()), 1)
		#define getrule2() (printf("found rule: [%s]\n", name.c_str()), 1)
		#define errtok2()  (throw parse_error(string("error at token: ") + tok.peek()))
		// built in rules
		// special case - literal rule
		if      (name.length() >= 2 && name[0] == '$') return tok.peek() == name.substr(1) ? gettok2() : errtok2();
		else if (name == "eol")  return tok.peek() == tok.TOK_EOL ? gettok2() : errtok2();
		else if (name == "eof")  return tok.peek() == tok.TOK_EOF ? 1 : errtok2();
		else if (name == "endl") return tok.peek() == tok.TOK_EOL || tok.peek() == tok.TOK_EOF ? gettok2() : errtok2();
		// check rule existance
		if (rules.count(name) == 0) throw parse_error("missing rule: " + name);
		const auto& rule = rules[name];
		// run rule by rule type
		if (rule.type == "get") {
			for (auto& sr : rule.subrules)
				runrule2(sr);
			return getrule2();
		}
		else if (rule.type == "peek") {
			int pos = tok.pos;
			// string rname;
			try {
				for (auto& sr : rule.subrules)
					// runrule2(rname = sr);
					runrule2(sr);
			}
			catch (parse_error& e) {
				// printf("error peeking rule [%s]: %s\n", rname.c_str(), e.what());
				printf("error peeking rule [%s]: %s\n", rule.name.c_str(), e.what());
				tok.pos = pos;
				return 0;
			}
			printf("peek rule [%s] ok\n", rule.name.c_str());
			tok.pos = pos;
			return 1;
		}
		else if (rule.type == "mul") {
			int found = 1;
			while (found) {
				found = 0;
				for (auto& sr : rule.subrules)
					if (runrule2(sr)) found = 1;
			}
			return 1;
		}
		else if (rule.type == "any") {
			for (auto& sr : rule.subrules)
				if (runrule2(sr)) return 1;
			return 0;
		}
		else if (rule.type == "if") {
			if      (rule.subrules.size() != 2) throw parse_error("rule-type if requires 2 subrules, found " + to_string(rule.subrules.size()));
			else if (runrule2(rule.subrules[0])) return runrule2(rule.subrules[1]), 1;
			else    return 0;
		}
		// rule type not found
		throw parse_error("unknown rule-type: " + rule.type);
	}

	void setup() {
		addrule("prog", "mul", "function emptyline");
		addrule("emptyline", "get", "eol");
		addrule("function", "if", "function_start function_main");
		addrule("function_start", "get", "$function");
		addrule("function_main", "get", "$function $( $) eol function_body $end $function");
		addrule("function_body", "mul", "emptyline");

		showrules();
	}
	
	void setup2() {
		addrule("prog", "mul", "hello eol");
		addrule("hello", "get", "$hello $world");
		showrules();
	}
};