#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <unistd.h>

#include "vigenere.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

using namespace vigenere;

enum class COMMAND {
	ENCODE,
	DECODE,
};

int main(int argc, char **argv)
{
	char c;
	COMMAND cmd;
	string raw_buf, in_key;
	const char *filename;

	if (argc < 2) {
		static std::stringstream ss;
		ss << "Usage: " << argv[0] << " [options] file" << endl << 
		"Options:" << endl <<
		"\t -e encode" << endl <<
		"\t -d decode" << endl <<
		"\t -k key (relevant for encode)" << endl;

		cout << ss.str();
		return 0;
	}

	while ((c = getopt (argc, argv, "e:d:k:")) != -1)
		switch (c) {
		case 'e':
			cmd = COMMAND::ENCODE;
			filename = optarg;
			cout << "ENCODE " << filename << endl; 
			break;
		case 'd':
			cmd = COMMAND::DECODE;
			filename = optarg;
			cout << "DECODE " << filename << endl;
			break;
		case 'k':
			in_key.assign(optarg);
			cout << "Encoding with key: " << in_key << endl;
			break;
		case '?':
			if (optopt == 'e' || optopt == 'd' || optopt == 'k')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf (stderr, "Unknown option '-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character '%d'.\n",	optopt);
			return 1;
		default:
			abort();
		}

	if (optind != argc) {
		cerr << "Non-option arguments are present." << endl;
		return 1;
	}

	if (vigenere::read_file(filename, raw_buf)) {
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
