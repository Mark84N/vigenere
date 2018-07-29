#ifndef VIGENGERE_HPP
#define VIGENGERE_HPP

#include <string>

#define MAX_KEY_LEN 12 /* known beforehand */

namespace vigenere
{

int read_file(const char *filename, std::string &buf);
int get_key_len(std::string &out);
std::string get_key_by_freq(std::string &buf, int key_len);
std::string decode(std::string &buf, std::string &key);

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

	return 'A' + ((ch_idx + shift) % 25);
}

};

#endif