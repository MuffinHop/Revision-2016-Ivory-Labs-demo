#include "readShader.hpp"

char* readShader(const char* filename) {
	fstream input(filename);
	string line;
	string text;
	int linenum = 0;

	cout << "---------------------\n";
	cout << "Opening shader file: ";
	cout << filename;
	cout << "\n";
	cout << "---------------------\n";
	cout << "\n";
	
	while (getline(input, line)) {
		/*
		cout << linenum++;
		cout << " : ";
		cout << line;
		cout << "\n";
		*/
		text.append(line);
		text.append("\n");
	}
	cout << "\n";
	cout << "\n";
	cout << "\n";
	char *str = new char[text.length() + 1];
	strcpy(str, text.c_str());
	return str;
}