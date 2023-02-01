#include <iostream>
using namespace std;
#include "tokenizer.hpp"
#include "ruleset.hpp"
// #include "lang1.hpp"


struct Lang2 : Ruleset {
	vector<string> lines;

	Lang2() {
		addrules({
			{ "prog", "*assign !eof" },
			{ "emptyline", "eol" },
			{ "keywords", "dim function if while for end print" },
			// variables
			{ "assign", "ident !$= !expr !eol" },
			// { "expr", "_value | _varpath" },
			{ "expr", "$null | number | strlit" },
			// { "_varpath", "ident" }
		});
	}


	// virtual void gettoken(const string& rule, const string& token) {
	// 	// Ruleset::gettoken(token);
	// 	if      (state() == "assign" && rule == "ident")  lines.push_back("assign :: " + token + " = ");
	// 	else if (state() == "expr" && laststate() == "assign") lines.back() += token;
	// }
	// virtual void state_start() {
	// 	Ruleset::state_start();
	// }
	// virtual void state_end() {
	// 	Ruleset::state_end();
	// }
};


void lang2_test() {
	Lang2 l2;
	l2.tok.loads(
		"a = 102\n"
		"b = \"ass\"\n"
		"fart"
	);
	l2.runall();
	l2.parse_result.show();
}


int main() {
	// lang1_test();
	lang2_test();
}