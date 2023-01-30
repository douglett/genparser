#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"


struct Prog {
	struct Value   { string type; int value; };
	struct Dim     { string name; };
	struct Assign  { string name; Value value; };
	struct Print   { string name; };
	struct Line    { string type; int index; };
	vector<string> strings;
	vector<Dim>    gdims;
	vector<Assign> assigns;
	vector<Print>  prints;
	vector<Line>   lines;
};

struct Lang1 : Ruleset::Hook {
	Ruleset rs;
	Prog prog;

	Lang1() {
		rs.addrules({
			{ "prog", "*dim *assign *print !eof" },
			{ "emptyline", "eol" },
			{ "keywords", "dim function if while for end print" },
			// function
			// { "function", "$function !ident !$( !$) !endl !function_end" },
			// { "function_end", "$end $function endl"},
			// statements
			{ "print", "$print !ident !endl" },
			// variables
			{ "dim", "$dim !ident !eol" },
			{ "assign", "ident !$= !_expr !eol" },
			{ "_expr", "_value | _varpath" },
			{ "_value", "$null | number | strlit" },
			{ "_varpath", "ident" }
		});
		rs.hook = this;
	}

	void testinput1() {
		rs.tok.loads(
			"dim a\n"
			"a = 102\n"
			"b = \"ass\"\n"
			"print a\n"
			// "function foo()\n"
			// "end function"
		);
	}

	int parse() {
		int result = rs.runrule("prog");
		printf("parse result: %d\n", result);
		return result;
	}

	virtual void hooktoken(const string& rule, const string& token) {
		Ruleset::Hook::hooktoken(rule, token);

		if      (rs.getstate() == "dim" && rule == "ident")     prog.gdims.push_back({ token }), prog.lines.push_back({ "dim", int(prog.gdims.size()-1) });
		// 
		else if (rs.getstate() == "assign" && rule == "ident")  prog.assigns.push_back({ token }), prog.lines.push_back({ "assign", int(prog.assigns.size()-1) });
		else if (rs.getstate() == "assign" && rule == "$null")  prog.assigns.back().value = { "null" };
		else if (rs.getstate() == "assign" && rule == "number") prog.assigns.back().value = { "number", stoi(token) };
		else if (rs.getstate() == "assign" && rule == "strlit") prog.strings.push_back(stripstrlit(token)), prog.assigns.back().value = { "string", int(prog.strings.size()-1) };
		// 
		else if (rs.getstate() == "print" && rule == "$print")  prog.prints.push_back({ }), prog.lines.push_back({ "print", int(prog.prints.size()-1) });
		else if (rs.getstate() == "print" && rule == "ident")   prog.prints.back().name = token;
	}
};

struct Val {
	enum VAL_TYPE {
		VT_NULL = 0,
		VT_INT,
		VT_STRING
	};
	VAL_TYPE type = VT_NULL;
	int i = 0;
	string s;
	Val()         { type = VT_NULL; }
	Val(int v)    { type = VT_INT, i = v; }
	Val(string v, int strip=0) { 
		type = VT_STRING, s = v;
		if (strip) s = s.substr(1, s.length()-2);
	}
	string to_string() {
		switch (type) {
		case VT_NULL:    return "null";
		case VT_INT:     return std::to_string(i);
		case VT_STRING:  return '"' + s + '"';
		}
		return "bad_type";
	}
};

struct Runtime {
	Prog prog;
	int lno = 0;
	map<string, Val> vars;
	
	int run() {
		for (auto& ln : prog.lines)
			if      (ln.type == "dim")    vars[prog.gdims[ln.index].name] = Val();
			else if (ln.type == "assign") {
				if      (prog.assigns[ln.index].value.type == "null")   vars[prog.assigns[ln.index].name] = Val();
				else if (prog.assigns[ln.index].value.type == "number") vars[prog.assigns[ln.index].name] = Val(prog.assigns[ln.index].value.value);
				else if (prog.assigns[ln.index].value.type == "string") vars[prog.assigns[ln.index].name] = Val(prog.strings[prog.assigns[ln.index].value.value]);
			}
			else if (ln.type == "print")  printf("> %s\n", vars[prog.prints[ln.index].name].to_string().c_str());
		return 0;
	}
};


int lang1_test() {
	Lang1 l1;
	l1.testinput1();
	l1.parse();
	for (const auto& ln : l1.prog.lines)
		printf(" %-10s  %d\n", ln.type.c_str(), ln.index);
	Runtime r = { l1.prog };
	r.run();
	cout << r.vars["a"].to_string() << endl;
	cout << r.vars["b"].to_string() << endl;
	return 0;
}