#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"
// #include "lang1.hpp"


struct Var {
	enum TYPE { T_NULL=0, T_NUMBER, T_STRING };
	TYPE type = T_NULL;
	int i = 0;
	string s;

	string to_string() {
		switch (type) {
		case T_NULL:    return "null";
		case T_NUMBER:  return std::to_string(i);
		case T_STRING:  return s;
		}
		throw runtime_error("bad_type");
	}
};


struct Lang2 : Ruleset {
	// vector<string> lines;

	Lang2() {
		addrules({
			{ "prog", "*statement !eof" },
			{ "emptyline", "eol" },
			{ "keywords", "print end if else" },
			// statements
			{ "statement", "assign | print | if | emptyline" },
			{ "print", "$print *expr !endl" },
			{ "if", "$if !expr !$then !endl !block ?else !$end !$if !endl" },
			// { "elseif", "$else $if !expr !endl !block"},
			{ "else", "$else !endl !block"},
			{ "block", "*statement" },
			// variables
			{ "assign", "varpath !$= !expr !eol" },
			{ "varpath", "ident" },
			{ "expr", "$null | $true | $false | number | strlit | varpath" },
		});
	}


	map<string, Var> vars;

	void run() {
		// for (const auto& n : parse_result.get("prog").list)
		// 	if (n.rule == "statement") run_statement(n);
		run_block(parse_result.get("prog"));
	}

	void run_block(const Node& block) {
		for (auto& n : block.list)
			run_statement(n);
	}

	void run_statement(const Node& stmt) {
		for (auto& n : stmt.list)
			if (n.rule == "emptyline") ;
			else if (n.rule == "print") {
				printf("> ");
				for (auto& nn : n.list)
					if (nn.rule == "expr")
						printf("%s ", run_expr(nn).to_string().c_str());
				printf("\n");
			}
			else if (n.rule == "assign") {
				auto& id = n.get("varpath").get("ident").value;
				auto  ex = run_expr(n.get("expr"));
				printf("%s = %s\n", id.c_str(), ex.to_string().c_str());
				vars[id] = ex;
			}
			else if (n.rule == "if") {
				if (run_expr_truthy(n.get("expr")))
					run_block(n.get("block"));
				else if (n.count("else"))
					run_block(n.get("else").get("block"));
			}
			else
				throw runtime_error("bad_statement [" + n.rule + "]");
	}

	int run_expr_truthy(const Node& n) {
		auto v = run_expr(n);
		switch (v.type) {
		case Var::T_NULL:    return false;
		case Var::T_NUMBER:  return !!v.i;
		default:             throw runtime_error("value not truthy [" + v.to_string() + "]");
		}
	}

	Var run_expr(const Node& n) {
		auto& v = n.list.at(0);
		if      (v.rule == "$null")    return { Var::T_NULL };
		else if (v.rule == "$true")    return { Var::T_NUMBER, 1 };
		else if (v.rule == "$false")   return { Var::T_NUMBER, 0 };
		else if (v.rule == "number")   return { Var::T_NUMBER, stoi(v.value) };
		else if (v.rule == "strlit")   return { Var::T_STRING, 0, stripstrlit(v.value) };
		else if (v.rule == "varpath")  return vars.count(v.get("ident").value) ? vars[v.get("ident").value] : Var{ Var::T_NULL };
		throw runtime_error("bad_expression [" + v.rule + "]");
	}

};


void lang2_test() {
	Lang2 l2;
	// l2.tok.loads(
	// 	"a = 102\n"
	// 	"b = \"ass\"\n"
	// 	"print \"hello world\" 10 null true\n"
	// 	"print a b\n"
	// );
	l2.tok.loadf("test.bas");
	l2.parse();
	l2.parse_result.show();

	l2.run();

	// printf("vars %s\n", l2.vars.at("a").to_string().c_str());
}


int main() {
	// lang1_test();
	lang2_test();
}