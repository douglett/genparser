#pragma once
#include <string>
#include <vector>
#include <sstream>
using namespace std;


struct parse_error : runtime_error { using runtime_error::runtime_error; };


vector<string> splitws(const string& str) {
	stringstream ss(str);
	string s;
	vector<string> vs;
	while (ss >> s) vs.push_back(s);
	return vs;
}

int vsfind(const vector<string>& vs, const string& s, size_t start=0) {
	for (int i = start; i < vs.size(); i++)
		if (vs[i] == s) return i;
	return -1;
}


// vector<string> splittok(const string& line) {
// 	vector<string> vs;
// 	string s;
// 	#define addtok() (s.length() ? vs.push_back(s), s = "" : s)
// 	for (int i = 0; i < line.length(); i++)
// 		if      (isspace(line[i])) addtok();
// 		else if (isalnum(line[i])) s += line[i];
// 		else    addtok(), s += line[i], addtok();
// 	addtok();
// 	return vs;
// }


int isident(const string& str) {
	if (str.length() == 0) return 0;
	if (!isalpha(str[0]) && str[0] != '_') return 0;
	for (int i = 1; i < str.length(); i++)
		if (!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_') return 0;
	return 1;
}