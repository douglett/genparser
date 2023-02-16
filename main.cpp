#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"


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
const Var VAR_TRUE  = { Var::T_NUMBER, 1 };
const Var VAR_FALSE = { Var::T_NUMBER, 0 };


struct Lang2 {
	Ruleset rules;

	Lang2() {
		rules.addrules({
			{ "prog", "*statement !eof" },
			{ "emptyline", "?comment eol" },
			{ "endl", "?comment eol | ?comment eof" },
			{ "keywords", "print end if else" },
			// statements
			{ "statement", "assign | print | if | emptyline" },
			{ "print", "$print *expr !endl" },
			{ "if", "$if !expr ?$then !endl !block *elseif ?else !$end !$if !endl" },
			{ "elseif", "$else $if !expr ?$then !endl !block"},
			{ "else", "$else !endl !block"},
			{ "block", "*statement" },
			// variables
			{ "assign", "varpath !$= !expr !eol" },
			{ "varpath", "ident" },
			// expressions
			{ "expr", "expr_comp" },
			// { "expr_or", "expr_and $| $| !expr_and | expr_and" },
			// { "expr_and", "expr_comp $& $& !expr_comp | expr_comp" },
			{ "expr_comp", "expr_add expr_comp_op !expr_add | expr_add" },
			{ "expr_comp_op", "$= $= | $! $= | $> $= | $< $= | $> | $<" },
			{ "expr_add", "expr_mul $+ !expr_mul | expr_mul $- !expr_mul | expr_mul" },
			{ "expr_mul", "expr_value $* !expr_value | expr_value $/ !expr_value | expr_value" },
			{ "expr_value", "$null | $true | $false | number | strlit | varpath" },
		});
	}


	map<string, Var> vars;

	void run() {
		// for (const auto& n : parse_result.get("prog").list)
		// 	if (n.rule == "statement") run_statement(n);
		run_block(rules.parse_result.get("prog"));
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
				// printf("%s = %s\n", id.c_str(), ex.to_string().c_str());
				vars[id] = ex;
			}
			else if (n.rule == "if") {
				int found = 0;
				if (run_expr_truthy(n.get("expr")))  // if
					run_block(n.get("block")), found = 1;
				for (int i = 0; !found && i < n.count("elseif"); i++)  // else-if
					if (run_expr_truthy(n.get("elseif", i).get("expr")))
						run_block(n.get("elseif", i).get("block")), found = 1;
				if (!found && n.count("else"))  // else
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
		// basic expressions
		if      (n.rule == "$null")    return { Var::T_NULL };
		else if (n.rule == "$true")    return { Var::T_NUMBER, 1 };
		else if (n.rule == "$false")   return { Var::T_NUMBER, 0 };
		else if (n.rule == "number")   return { Var::T_NUMBER, stoi(n.value) };
		else if (n.rule == "strlit")   return { Var::T_STRING, 0, stripstrlit(n.value) };
		else if (n.rule == "varpath")  return vars.count(n.get("ident").value) ? vars[n.get("ident").value] : Var{ Var::T_NULL };
		// compound expressions
		else if (n.rule == "expr")        return run_expr(n.list.at(0));
		else if (n.rule == "expr_value")  return run_expr(n.list.at(0));
		// else if (n.rule == "expr_or") {
		// 	auto a = run_expr(n.get("expr_and", 0));
		// 	if (n.count("expr_and") == 1)  return a;
		// 	auto b = run_expr(n.get("expr_and", 1));
		// }
		else if (n.rule == "expr_comp") {
			auto a = run_expr(n.get("expr_add", 0));
			if (n.count("expr_add") == 1)  return a;
			auto b = run_expr(n.get("expr_add", 1));
			auto oplist = n.get("expr_comp_op").list;
			auto c = oplist.at(0).value + (oplist.size() > 1 ? oplist.at(1).value : "");
			// printf("comp: [%s]\n", c.c_str());
			if (a.type == Var::T_NUMBER && b.type == Var::T_NUMBER) {
				if (c == "==")  return a.i == b.i ? VAR_TRUE : VAR_FALSE;
				if (c == "!=")  return a.i != b.i ? VAR_TRUE : VAR_FALSE;
				if (c == ">=")  return a.i >= b.i ? VAR_TRUE : VAR_FALSE;
				if (c == "<=")  return a.i <= b.i ? VAR_TRUE : VAR_FALSE;
				if (c == ">" )  return a.i >  b.i ? VAR_TRUE : VAR_FALSE;
				if (c == "<" )  return a.i <  b.i ? VAR_TRUE : VAR_FALSE;
			}
		}
		else if (n.rule == "expr_add") {
			auto a = run_expr(n.get("expr_mul", 0));
			if (n.count("expr_mul") == 1)  return a;
			auto b = run_expr(n.get("expr_mul", 1));
			auto c = n.list.at(1).value;
			if (a.type == Var::T_NUMBER || b.type == Var::T_NUMBER) {
				if (c == "+")  return { Var::T_NUMBER, a.i + b.i };
				if (c == "-")  return { Var::T_NUMBER, a.i - b.i };
			}
		}
		else if (n.rule == "expr_mul") {
			auto a = run_expr(n.get("expr_value", 0));
			if (n.count("expr_value") == 1)  return a;
			auto b = run_expr(n.get("expr_value", 1));
			auto c = n.list.at(1).value;
			if (a.type == Var::T_NUMBER || b.type == Var::T_NUMBER) {
				if (c == "*")  return { Var::T_NUMBER, a.i * b.i };
				if (c == "/")  return { Var::T_NUMBER, a.i / b.i };
			}
		}
		// fail
		throw runtime_error("bad_expression [" + n.rule + "]");
	}

};


void lang2_test() {
	Lang2 l2;
	l2.rules.tok.loadf("test.bas");
	l2.rules.parse();
	l2.rules.parse_result.show();

	l2.run();
}


int main() {
	// lang1_test();
	lang2_test();
}