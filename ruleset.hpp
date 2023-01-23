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
		string name;
		vector<string> subrules;
	};

	map<string, Rule> rules;
	Tokenizer tok;

	void showrules() {
		for (const auto& r : rules) {
			printf("%s\n", r.second.name.c_str());
			for (const auto& sub : r.second.subrules)
				printf("\t> %s\n", sub.c_str());
		}
	}

	void addrule(const string& name, const string& subrules) {
		if (rules.count(name) == 1) throw parse_error("duplicate rule definition: " + name);
		rules[name] = { name, splitws(subrules) };
		if (rules[name].subrules.size() == 0) throw parse_error("no subrules in rule: " + name);
	}

	void addrules(const vector<array<string, 2>>& rulelist) {
		for (auto& rs : rulelist)
			addrule(rs[0], rs[1]);
	}

	int runrule(const string& name) {
		int pos = tok.pos;
		#define gettok()    ( tok.get(), printf("found token: [%s]\n", name.c_str()), 1 )
		#define getrule()   ( printf("found rule: [%s]\n", name.c_str()), 1 )
		#define unget()     ( tok.pos = pos, 0 )
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
		const auto& sr = rules[name].subrules;
		for (int i = 0, t; i < sr.size(); i++)
			if      (sr[i] == "|") break;  // all rules between 'or' markers found
			else if (runrule(sr[i])) ;  // rule found
			else if ((t = vsfind(sr, "|", i)) > -1) i = t;  // search for next 'or' markers
			else    return unget();  // rule not found
		return getrule();
	}
};