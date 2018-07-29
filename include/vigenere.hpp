#ifndef VIGENGERE_HPP
#define VIGENGERE_HPP

#include <string>
#include <algorithm>
#include <cctype>

#define MAX_KEY_LEN 12 /* known beforehand */

namespace vigenere
{
using std::string;

int read_file(const char *filename, string &buf);
int get_key_len(const string &out);
string get_key_by_freq(const string &buf, int key_len);
string normalize(const string &raw);
string decode(const string &buf, const string &key);
string encode(const string &buf, const string &key);

static inline int alpha_to_idx(int ch)
{
	return ch - 'A';
}

static inline char shift_left(char ch, int shift)
{
	int ch_idx = alpha_to_idx(ch);

	return 'A' + (ch_idx >= shift? ch_idx - shift :
									(ch_idx + (26 - shift)) % 25);
}

static inline char shift_right(char ch, int shift)
{
	int ch_idx = alpha_to_idx(ch);

	return 'A' + (ch_idx + shift <= 25? ch_idx + shift : 
								((ch_idx + shift) % 25) - 1);
}

static string remove_not_alpha(const string &buf)
{
	string out(buf.size(), 0);
	std::remove_copy_if(buf.begin(), buf.end(), out.begin(),
				[](char c) { return !isalpha(c); });
	return out;
}

};

#endif