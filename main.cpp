#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"

void test4() {
	Ruleset rs;
	rs.addrules({
		// { "prog", "$a | $b $b | $c" },
		{ "prog", "*function !eof" },
		{ "emptyline", "eol" },
		// function
		{ "function", "$function !ident !$( !$) !endl !function_end" },
		{ "function_end", "$end $function endl"}
	});
	rs.tok.loads(
		// "b b"
		"function foo()\n"
		"end function"
	);

	int result = rs.runrule("prog");
	printf("parse result: %d\n", result);
}

int main() {
	test4();
}