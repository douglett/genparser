#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"

void test4() {
	Ruleset rs;
	rs.addrules({
		// { "prog", "$a | $b $b | $c" },
		{ "prog", "*dim *assign *function !eof" },
		{ "emptyline", "eol" },
		{ "keywords", "dim function if while for end" },
		// function
		{ "function", "$function !ident !$( !$) !endl !function_end" },
		{ "function_end", "$end $function endl"},
		// variables
		{ "dim", "$dim !ident !eol" },
		{ "assign", "ident !$= !number !eol" }
	});
	rs.tok.loads(
		"dim a\n"
		"a = 102\n"
		"function foo()\n"
		"end function"
	);
	struct Hook2 : Ruleset::Hook {
		virtual void parsehook(const string& rule, const string& token) const {
			printf("hook action: %s :: [%s]\n", rule.c_str(), token.c_str());
		}
	};
	Hook2 h;
	rs.hook = &h;

	int result = rs.runrule("prog");
	printf("parse result: %d\n", result);
}

int main() {
	test4();

}