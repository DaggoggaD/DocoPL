#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/any.hpp>
#include <typeinfo>
using namespace std;

struct token
{
	string T_Type;
	boost::any T_Value;
	string T_DISPLAY;
};

string T_STR = "T_STR";
string T_INT = "T_INT";
string T_FLOAT = "T_FLOAT";
string T_ARRAY = "T_ARRAY";
string T_KW = "T_KW";
string T_ID = "T_ID";
string T_OP = "T_OP";
//momentary
string T_MOM = "T_MOM";

vector<string> startStopChars = {
	"(",
	R"(")",
	")",
	"{",
	"}",
	"[",
	"]",
	".",
	"+",
	"-",
	"/",
	"*",
	">",
	"<",
	";",
	" ",
	",",
	"\t"
};

vector<string> keywords = {
	"store",
	"if",
	"for",
	"func",
	"print",
	"dtin",
	"return"
};

vector<string> operators = {
	"=",
	"+",
	"-",
	"*",
	"**",
	"/",
	"&&",
	"||",
	"%",
	"(",
	")",
	"[",
	"]",
	"{",
	"}",
	".",
	";",
	","
};

bool isInt(string line)
{
	char* p;
	strtol(line.c_str(), &p, 10);
	return *p == 0;
}

bool isFloat(string line)
{
	char* p;
	strtod(line.c_str(), &p);
	return *p == 0;
}

bool isNum(string line) {
	if (isInt(line) == 1 or isFloat(line) == 1) {
		return 1;
	}
}

//strs as the vector containing elements to be searched
//toFind as the char to find
bool find_char_in_arr(vector<string> strs, char toFind) {
	for (int currc = 0; currc < strs.size(); currc++) {
		if (strs[currc] == string(1, toFind)) {
			return true;
		}
	}
	return false;
}

bool find_string_in_arr(vector<string> strs, string toFind) {
	for (int currc = 0; currc < strs.size(); currc++) {
		if (strs[currc] == toFind) {
			return true;
		}
	}
	return false;
}

//used in @token_indexer to advance word
void f_next_word(int* ID, string* curr, string* nextv, vector<string> arr, int closed) {
	if (closed == 0) {
		int remaining = arr.size() - *ID;
		if (remaining >= 1) {
			*ID += 1;
			*curr = arr[*ID];
			if (remaining > 1) {
				*nextv = arr[*ID + 1];
			}
			else {
				*nextv = string(NULL);
			}
		}
	}
}


//divides word in tokens, T_KW, T_OP, T_ID, T_FLOAT, T_INT
vector<token> token_indexer(vector<string> token_not_indexed) {
	vector<token> tokens;
	int currID = -1;
	string curr_word;
	string next_word;
	int instring = 0;
	int inarr = 0;
	//cycles until END_FILE is detected
	f_next_word(&currID, &curr_word, &next_word, token_not_indexed, 0);
	while (curr_word != "END_FILE") {
		for (int chid = 0; chid < curr_word.size(); chid++) {
			if (curr_word[chid]==char(" "))
			{
				curr_word.erase(curr_word.begin() + chid);
			}
		}
		int close = 0;
		if (next_word == "END_FILE" or curr_word=="END_FILE") {
			close = 1;
		}
		else {
			//checks if curr_word is in keywords
			if (find(keywords.begin(), keywords.end(), curr_word) != keywords.end()) {
				token tok;
				tok.T_Type = T_KW;
				tok.T_Value = curr_word;
				tok.T_DISPLAY = curr_word;
				tokens.push_back(tok);
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
			}
			//checks for string
			else if (curr_word == R"(")") {
				string buildstr;
				if (instring == 0) { instring += 1; }
				else { instring -= 1; }
				while (next_word != R"(")")
				{
					buildstr += next_word;
					f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
				}
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
				token tok;
				tok.T_Type = T_STR;
				tok.T_Value = buildstr;
				tok.T_DISPLAY = buildstr;
				tokens.push_back(tok);

				int instring = 0;
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
				
			}
			//checks for arrays
			else if (curr_word == "[") {
				inarr += 1;
				vector<string> inside_words;
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
				while (inarr != 0) {
					//increases or decreases array depth
					if (curr_word == "[") {
						inarr += 1;
					}
					else if (curr_word == "]")
					{
						inarr -= 1;
					}

					//adds element if not empty, goes to next word
					if (curr_word != " ") {
						inside_words.push_back(curr_word);
					}
					f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
				}

				//inside_words.erase(inside_words.end() - 1);
				inside_words.push_back("END_FILE");


				//for array display
				string display_string = "[";
				for (int l = 0; l < inside_words.size(); l++) {
					if (inside_words[l] != "END_FILE")
						display_string += inside_words[l];
				}
				

				vector<token> inside_toks = token_indexer(inside_words);
				token tok;
				tok.T_Type = T_ARRAY;
				tok.T_Value = inside_toks;
				tok.T_DISPLAY = display_string;
				tokens.push_back(tok);
			}
			//checks for numbers
			else if (isNum(curr_word) == 1) {
				string num = curr_word;
				token tok;
				if (next_word == ".") {
					f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
					num += curr_word;
					if (isNum(next_word) == 1 and next_word != ".") {
						f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
						num += curr_word;
					}
					tok.T_Type = T_FLOAT;
					tok.T_Value = stof(num);
					tok.T_DISPLAY = num;
				}
				else {
					tok.T_Type = T_INT;
					tok.T_Value = stoi(num);
					tok.T_DISPLAY = num;
				}

				tokens.push_back(tok);
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
			}
			//deletes empty spaces
			else if (find_string_in_arr(operators, curr_word)) {
				token tok;
				tok.T_Type = T_OP;
				tok.T_Value = curr_word;
				tok.T_DISPLAY = curr_word;
				tokens.push_back(tok);
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
			}
			else if (curr_word == " " or curr_word=="\t") {
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
			}
			else {
				token tok;
				tok.T_Type = T_ID;
				tok.T_Value = curr_word;
				tok.T_DISPLAY = curr_word;
				tokens.push_back(tok);
				f_next_word(&currID, &curr_word, &next_word, token_not_indexed, close);
			}
		}
			

		if (close == 1) {
			return tokens;
		}
	}
	return tokens;
}


//first separation of words in semi-tokens
//uses startstopchars to separate words based on determined chars
vector<string> inspect_line_chars(string line) {
	string currstr;
	char currchar;
	char nextchar;
	vector<string> tokens;
	for (int charId = 0; charId < line.size(); charId++) {
		currchar = line[charId];
		if (charId < line.size() - 1) {
			nextchar = line[charId + 1];
		}
		else {
			nextchar = '$';
		}

		if (!find_char_in_arr(startStopChars, currchar)) {
			if (find_char_in_arr(startStopChars, nextchar) || nextchar == '$') {
				currstr += currchar;
				tokens.push_back(currstr);
				currstr = "";
			}
			else {
				currstr += currchar;
			}

		}
		else {
			currstr += currchar;
			tokens.push_back(currstr);
			currstr = "";
		}

	}
	return tokens;
}

//cycles found lines, analyzing them
vector<string> cycle_lines(vector<string> lines) {
	vector<string> token_not_indexed;
	for (int i = 0; i < lines.size(); i++) {
		vector<string> tni_rows = inspect_line_chars(lines[i]);
		for (int n = 0; n < tni_rows.size(); n++) {
			token_not_indexed.push_back(tni_rows[n]);
		}
	}
	token_not_indexed.push_back("END_FILE");
	return token_not_indexed;
}

//reads file, dividing it into lines
vector<string> read_file(string filename) {
	cout << "reading file: _" << filename << "_" << endl;
	cout.flush();
	fstream new_file;
	// open a file to perform read operation using file object.
	new_file.open(filename, ios::in);

	vector<string> readArr;
	// Checking whether the file is open.
	if (new_file.is_open()) {
		string sa;
		// Read data from the file object and put it into a string.
		while (getline(new_file, sa)) {
			// Add data to Arr
			readArr.push_back(sa);
		}

		// Close the file object.
		new_file.close();
	}
	return readArr;
}

//displays tokens after categorization in @token_indexer
void display_toks(vector<token> toks) {
	for (int i = 0; i < toks.size(); i++) {
		cout << toks[i].T_Type << " : " << toks[i].T_DISPLAY << endl;
	}
}

//starts the cycle
vector<string> Lex(string filename) {
	setvbuf(stdout, NULL, _IONBF, 0);
	vector<string> rows = read_file(filename);
	vector<string> not_indexed_tokens = cycle_lines(rows);
	vector<token> result_tok = token_indexer(not_indexed_tokens);
	display_toks(result_tok);
	return not_indexed_tokens;
}