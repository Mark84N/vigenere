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

int main(int argc, char **argv)
{
	string     raw_buf;
	string		file_name;

	if (argc == 2) {
		file_name.assign(argv[1]);
	} else {
		cerr << "Incorrect number of arguments. Terminate." << endl;
		return 1;
	}

	if (vigenere::read_file(file_name.c_str(), raw_buf)) {
		cerr << "Reading failed, make sure file exists and not empty. Terminate." << endl;
		return 1;
	}

	string buf = vigenere::normalize(raw_buf);
	if (buf.empty()) {
		cerr << "No alphabet characters present. Terminate." << endl;
		return 1;
	}

	int key_len = vigenere::get_key_len(buf);
	cout << "Key len: " << key_len << endl;

	string key = vigenere::get_key_by_freq(buf, key_len);
	cout << "Key: " << key.c_str() << ", size " << key.size() << endl;

	cout << "Decode: " << vigenere::decode(buf, key) << endl;
	return 0;
}
