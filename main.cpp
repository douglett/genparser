#include <iostream>
using namespace std;
// #include "v1.hpp"
#include "tokenizer.hpp"
#include "ruleset.hpp"

// void test1() {
// 	Ruleset rs;
// 	rs.setup2();
// 	Tokenizer tok;
// 	tok.loads("hello world \nhello world");

// 	int result = rs.runrule("prog", tok);
// 	printf("parse result: %d\n", result);
// }

// void test2() {
// 	Ruleset rs;
// 	rs.setup();
// 	Tokenizer tok;
// 	tok.loads(
// 		"\n"
// 		"function f\n"
// 		"\n"
// 		"end function\n"
// 	);

// 	int result = rs.runrule("prog", tok);
// 	printf("parse result: %d\n", result);
// }

// void test3() {
// 	Ruleset rs;
// 	rs.addrules({
// 		{ "prog", "get", "prog_lines eof" },
// 		{ "prog_lines", "mul", "hello_try eol_try" },
// 		{ "eol_try", "peek", "eol" },
// 		// hello
// 		{ "hello_try", "if", "hello_start hello" },
// 		{ "hello_start", "peek", "$hello" },
// 		{ "hello", "get", "$hello $world endl" }
// 	});
// 	rs.tok.loads(
// 		"hello world\n"
// 	);

// 	int result = rs.runrule2("prog");
// 	printf("parse result: %d\n", result);
// }

void test4() {
	Ruleset rs;
	rs.addrules({
		// { "prog", "", "a | b | c" },

		{ "prog", "require", "*function !eof" },
		// { "progline", "mul", "function emptyline" },
		{ "emptyline", "request", "eol" },
		// function
		{ "function", "", "$function !ident !$( !$) !endl" },
		// { "function", "request", "$function function_header function_body function_end" },
		// { "function_header", "require", "ident $( $) endl" },
		// { "function_body", "mul", "emptyline" },
		// { "function_end", "require", "$end $function endl" },
	});
	rs.tok.loads(
		"function foo()"
		// "end function"
	);

	int result = rs.runrule("prog");
	printf("parse result: %d\n", result);
}

int main() {
	// test1();
	// test2();
	// test3();
	test4();
	printf("%d\n", isident("foo"));
}