#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <algorithm>
#include <cstring>

#include "vigenere.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

using namespace vigenere;

static string remove_not_alpha(const string &buf)
{
	string out(buf.size(), 0);
	std::remove_copy_if(buf.begin(), buf.end(), out.begin(),
				[](char c) { return !isalpha(c); });
	return out;
}

int main(int argc, char **argv)
{
	string     raw_buf;
	string		file_name;

	if (argc == 2) {
		file_name.assign(argv[1]);
	} else {
		cerr << "Incorrect number of arguments, terminate." << endl;
		return 1;
	}

	if (vigenere::read_file(file_name.c_str(), raw_buf)) {
		cerr << "Reading failed, terminate." << endl;
		return 1;
	}

	string buf(std::move(remove_not_alpha(raw_buf)));

	if (buf.empty()) {
		cerr << "Empty file supplied or no alphabet characters present, terminate." << endl;
		return 1;
	}

	std::for_each(buf.begin(), buf.end(), [](char &ch) { if (isalpha(ch)) ch = toupper(ch); });
	buf.resize(strlen(buf.c_str()));

	int key_len = get_key_len(buf);
	cout << "Key len: " << key_len << endl;

	string key = get_key_by_freq(buf, key_len);
	cout << "Key: " << key.c_str() << ", size " << key.size() << endl;

	cout << "Decode: " << decode(buf, key) << endl;
	return 0;
}
