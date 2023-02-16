#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
// #include "helpers.hpp"
using namespace std;


struct Tokenizer {
	const string TOK_EOF = "$EOF";
	const string TOK_EOL = "$EOL";
	vector<string> tokens;
	int pos = 0;
	int lno = 1;

	int loadf(const string& fname) {
		tokens = {}, pos = 0, lno = 1;
		fstream fs(fname, ios::in);
		string s;
		if (!fs.is_open())
			return fprintf(stderr, "error opening file: %s\n", fname.c_str()), 1;
		while (getline(fs, s))
			tokenizeline(s);
		return 0;
	}

	int loads(const string& prog) {
		tokens = {}, pos = 0, lno = 1;
		stringstream ss(prog);
		string s;
		while (getline(ss, s))
			tokenizeline(s);
		return 0;
	}
	
	string getlit(const string& line, int& i) {
		string s = "\"";
		for (++i ; i < line.length(); i++) {
			s += line[i];
			if (line[i] == '"') break;
		}
		return s;
	}

	void tokenizeline(const string& line) {
		string s;
		// helpers
		#define addtok()  (s.length() ? tokens.push_back(s), s = "" : s)
		#define addchr(c) (tokens.push_back(string(1, c)))
		// parse
		for (int i = 0; i < line.length(); i++)
			if      (isspace(line[i])) addtok();
			else if (isalnum(line[i])) s += line[i];
			else if (line.substr(i, 2) == "//") addtok(), s = line.substr(i), i += s.length(), addtok();
			else if (line[i] == '"') addtok(), s = getlit(line, i), addtok();
			else    addtok(), addchr(line[i]);
		// finish up
		addtok();
		tokens.push_back(TOK_EOL);
	}

	const string& peek(int p=0) {
		return pos + p >= tokens.size() ? TOK_EOF : tokens[pos + p];
	}

	const string& get() {
		// return pos >= tokens.size() ? TOK_EOF : tokens[pos++];
		if (pos > tokens.size()) return TOK_EOF;
		if (tokens[pos] == TOK_EOL) lno++;
		return tokens[pos++];
	}
};