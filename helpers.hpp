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

string vsjoin(const vector<string>& vs, const string& glue=" ") {
	string s;
	for (int i = 0; i < vs.size(); i++)
		s += (i == 0 ? "" : glue) + vs[i];
	return s;
}

int vsfind(const vector<string>& vs, const string& s, size_t start=0) {
	for (int i = start; i < vs.size(); i++)
		if (vs[i] == s) return i;
	return -1;
}

int isstrlit(const string& str) {
	return str.length() >= 2 && str.front() == '"' && str.back() == '"';
}

int iscomment(const string& str) {
	return str.length() >= 2 && str[0] == '/' && str[1] == '/';
}

int isident(const string& str) {
	if (str.length() == 0) return 0;
	if (!isalpha(str[0]) && str[0] != '_') return 0;
	for (int i = 1; i < str.length(); i++)
		if (!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_') return 0;
	return 1;
}

int isnumber(const string& str) {
	if (str.length() == 0) return 0;
	for (int i = 0; i < str.length(); i++)
		if (!isdigit(str[i])) return 0;
	return 1;
}

string stripstrlit(const string& str) {
	return isstrlit(str) ? str.substr(1, str.length()-2) : str;
}