#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

// vector<string> rules = {
// 	"'function ident endl"
// };

// struct rule {
// 	string name;
// 	string entry;
// };

// vector<rule> rules = {
// 	{ "prog" }
// }

// prog: ( 'function & function ) | ( 'endl )
// function: 'function ident arglist endl
// arglist: '( ident ( ', ident | ') )

// prog: *( function endl )

// prog:
// 	any
// 		function
// 		endl

// function:
// 	$function
// 	ident
// 	arglist
// 	endl

// arglist:
// 	$(
// 	|
// 		():0
// 			$)
// 		():1
// 			ident 
// 			|
// 				():0
// 					$)
// 				():1
// 					$, ident


struct node {
	string value;
	vector<node> kids;
};

vector<node> rules;

stringstream ss;

int isident(const string& str) {
	if (str.length() == 0) return 0;
	if (!isalpha(str[0]) && str[0] != '_') return 0;
	for (int i = 1; i < str.length(); i++)
		if (!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_') return 0;
	return 1;
}

// int parserule(const int lineno, const string& rulestr) {
// 	ss.str(rulestr), ss.clear();
// 	string name;
// 	// rule name
// 	ss >> name;
// 	if (name.length() < 2 || name.back() != ':' || !isident(name.substr(0, name.length()-1)))
// 		return fprintf(stderr, "invalid rule name, line %d", lineno), 1;
// 	printf("name: %s\n", name.c_str());
// 	// 
// 	// ok
// 	return 0;
// }

struct parse_error : runtime_error { using runtime_error::runtime_error; };

void addtok(node& n, string& tok) {
	if (tok.length()) {
		// printf("here [%s]\n", tok.c_str());
		n.kids.push_back({ tok }), tok = "";
	}
}

node parselist(const string& line, int& i) {
	node n = { "()" };
	string tok;
	for ( ; i < line.length(); i++)
		if      (isspace(line[i])) addtok(n, tok);
		else if (line[i] == ':') addtok(n, tok), tok += line[i], addtok(n, tok);
		else if (line[i] == ')') return addtok(n, tok), n;
		else if (line[i] == '(') n.kids.push_back(parselist(line, ++i));
		else    tok += line[i];
	addtok(n, tok);
	return n;
}

node parseline(const string& line) {
	int pos = 0;
	node n = parselist(line, pos);
	if (pos < line.length())
		throw parse_error(string("line parsing terminated early, at position: ") + to_string(pos));
	return n;
}

void shownode(const node& n, int ind = 0) {
	printf("%s%s\n", string(ind*3, ' ').c_str(), n.value.c_str());
	for (auto& nn : n.kids)
		shownode(nn, ind+1);
}

int main() {
	printf("hello world\n");

	// parserule(1, "function: $function");
	node n = parseline("function: $function (hello world) ()");
	shownode(n);
}