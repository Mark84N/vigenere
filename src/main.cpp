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

enum class COMMAND {
	ENCODE,
	DECODE,
};

int main(int argc, char **argv)
{
	char c;
	COMMAND cmd;
	string rdbuf, in_key;
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
			break;
		case 'd':
			cmd = COMMAND::DECODE;
			filename = optarg;
			break;
		case 'k':
			in_key.assign(optarg);
			break;
		case '?':
			if (optopt == 'e' || optopt == 'd' || optopt == 'k')
				cerr << "Option '-" << optopt << "'' requires an argument." << endl;
			else if (isprint(optopt))
				cerr << "Unknown option '-" << optopt << "'." << endl;
			else
				cerr << "Unknown option character '-" << int(optopt) << "'." << endl;
			return 1;
		default:
			abort();
		}

	if (optind != argc) {
		cerr << "Non-option arguments are present." << endl;
		return 1;
	}

	if (cmd == COMMAND::ENCODE && in_key.empty()) {
		cerr << "Encoding requires a key (-k). Terminate." << endl;
		return 1;
	}

	if (vigenere::read_file(filename, rdbuf)) {
		cerr << "Reading failed, make sure file exists and not empty. Terminate." << endl;
		return 1;
	}

	if (cmd == COMMAND::DECODE) {
		int key_len;
		string buf, key;

		buf = vigenere::normalize(rdbuf);
		if (buf.empty()) {
			cerr << "No alphabet characters present. Terminate." << endl;
			return 1;
		}

		key_len = vigenere::get_key_len(buf);
		key = vigenere::get_key_by_freq(buf, key_len);
		cout << "Key: " << key << endl;
		cout << "Decode: " << vigenere::decode(buf, key) << endl;
	} else if (cmd == COMMAND::ENCODE) {
		cout << vigenere::encode(rdbuf, in_key) << endl;
	}

	return 0;
}
