#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"

struct Prog {
	struct Dim { string name; };
	struct Assign { string name; int value; };
	struct Print { string name; };
	struct Line { string type; int index; };
	vector<Dim> gdims;
	vector<Assign> assigns;
	vector<Print> prints;
	vector<Line> lines;
};

struct Runtime {
	Prog prog;
	int lno = 0;
	map<string, int> vars;
	
	int run() {
		for (auto& ln : prog.lines)
			if      (ln.type == "dim") vars[prog.gdims[ln.index].name] = 0;
			else if (ln.type == "print") printf("> %d\n", vars[prog.prints[ln.index].name]);
			else if (ln.type == "assign") vars[prog.assigns[ln.index].name] = prog.assigns[ln.index].value;
		return 0;
	}
};

struct Lang1 : Ruleset::Hook {
	Ruleset rs;
	Prog prog;

	Lang1() {
		rs.addrules({
			// { "prog", "$a | $b $b | $c" },
			{ "prog", "*dim *assign *print *function !eof" },
			{ "emptyline", "eol" },
			{ "keywords", "dim function if while for end print" },
			// function
			{ "function", "$function !ident !$( !$) !endl !function_end" },
			{ "function_end", "$end $function endl"},
			// statements
			{ "print", "$print !ident !endl" },
			// variables
			{ "dim", "$dim !ident !eol" },
			{ "assign", "ident !$= !number !eol" },
			// { "_literal", "number | strlit" }
		});
		rs.hook = this;
	}

	void testinput1() {
		rs.tok.loads(
			"dim a\n"
			"a = 102\n"
			"print a\n"
			"function foo()\n"
			"end function"
		);
	}

	int parse() {
		int result = rs.runrule("prog");
		printf("parse result: %d\n", result);
		return result;
	}

	virtual void parsehook(const string& rule, const string& token) {
		Ruleset::Hook::parsehook(rule, token);

		if      (rs.getstate() == "dim" && rule == "ident") prog.gdims.push_back({ token }), prog.lines.push_back({ "dim", int(prog.gdims.size()-1) }); //, printf("  dim %s\n", token.c_str());
		// else if (rs.getstate() == "function" && rule == "ident") prog.
		else if (rs.getstate() == "assign" && rule == "ident") prog.assigns.push_back({ token }), prog.lines.push_back({ "assign", int(prog.assigns.size()-1) });
		else if (rs.getstate() == "assign" && rule == "number") prog.assigns.back().value = stoi(token);
		else if (rs.getstate() == "print" && rule == "ident") prog.prints.push_back({ token }), prog.lines.push_back({ "print", int(prog.prints.size()-1) });
	}
};


int main() {
	Lang1 l1;
	l1.testinput1();
	l1.parse();
	for (const auto& ln : l1.prog.lines)
		printf(" %-10s  %d\n", ln.type.c_str(), ln.index);
	Runtime r = { l1.prog };
	r.run();
}