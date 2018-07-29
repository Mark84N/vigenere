#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <algorithm>
#include <cstring>
#include <map>
#include <vector>
#include <deque>

#include "vigenere.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

/*
	@brief Reads a file into buffer

	@param filename - path to the file
	@param buf - a string buffer to read to
	@return 0 on success, non-zero on error
*/
int vigenere::read_file(const char *filename, string &buf)
{
	std::ifstream   file;
	std::streampos  file_length;

	file.open(filename, std::ios::in | std::ios::ate);
	if (!file.is_open())
		return 1;

	file_length = file.tellg();
	file.seekg(0, std::ios_base::beg);
	file.clear();

	if (file_length == 0)
		file.close();
		return 1;

	buf.resize(file_length);
	file.read(&buf[0], file_length);
	file.close();

	return 0;
}

/*
	@brief guesses a key length which was used for encoding

	@param buf - a string buffer with normalized ciphertext
	@return key length on success, -1 on error

	Note: expects input buffer to be normalized, i.e. only alphabetic
	symbols uppercase, without punctuation, whitespaces and digits.
*/
int vigenere::get_key_len(string &buf)
{
	std::vector<int> match_per_shift;
	int sum = 0;

	if (buf.empty()) {
		return -1;
	}

	for (int shift = 1; shift < buf.length(); shift++) {
		for (int i = 0, j = shift; j < buf.length(); i++, j++) {
			if (buf[i] == buf[j]) {
				sum++;
			}
		}
		match_per_shift.push_back(sum);
		sum = 0;
	}

	/* find most succesful match in the interval of possible key length */
	auto max = std::max_element(match_per_shift.begin(), match_per_shift.begin() + MAX_KEY_LEN);
	int start_idx = std::distance(match_per_shift.begin(), max);

	/* split matches to 12 parts, to get intervals between most successful ones */
	std::vector<int> intervals_sum(MAX_KEY_LEN, 0);
	int matches_size = match_per_shift.size();
	int num_attempts = (matches_size / MAX_KEY_LEN); /* let the same num of attempts for each possible key len */
	for (int shift = 1; shift < MAX_KEY_LEN; shift++) {
		int i = 0;
		int j = start_idx;

		for ( ; i < num_attempts && j < matches_size; i++, j += shift) {
			intervals_sum[shift] += match_per_shift[j];
		}
	}

	auto it = std::max_element(intervals_sum.begin(), intervals_sum.end());
	int key_len = std::distance(intervals_sum.begin(), it);

	return key_len;
}

/*
	@brief Derives a key used for encryption

	@param buf - string with normalized ciphertext
	@param key_len - key length
	@return string with the key used for encryption

	Note: it is a very simple method for getting a key; basically it works
	on the assumption that the letter 'E' is the most  frequent in the original 
	text (as in any other English text); 'E's frequency is 12.02% across English.
	The closest is 'T' with 9.10% which make 'E' a well-defined choose.
	
	However, frequency analysis is very error-prone on small texts, and for better
	results,the interaction with user is needed.
*/
string vigenere::get_key_by_freq(string &buf, int key_len)
{
	std::vector<int> times_matched(25, 0);
	string key;

	for (int i = 0; i < key_len; i++) {
		for (int j = i; j < buf.length(); j += key_len) {
			times_matched[alpha_to_idx(buf[j])]++;
		}

		auto it = std::max_element(times_matched.begin(), times_matched.end());
		int idx = std::distance(times_matched.begin(), it);
		char encrypted = 'A' + idx;
		char ch = shift_left(encrypted, 'E' - 'A');
		key.push_back(ch);
		
		times_matched.assign(25, 0);
	}

	return key;
}

/*
	@brief Removes non-alpha characters from the buffer

	@param raw - string with the ciphertext
	@return string with normalized ciphertext
*/
std::string vigenere::normalize(std::string &raw)
{
	string buf(std::move(remove_not_alpha(raw)));

	std::for_each(buf.begin(), buf.end(), [](char &ch) { if (isalpha(ch)) ch = toupper(ch); });
	buf.resize(strlen(buf.c_str()));

	return buf;
}

/*
	@brief Decodes ciphertext with the given key

	@param buf - string with normalized ciphertext
	@param key - string with the key that will be used for decryption
	@return string with decoded text
*/
std::string vigenere::decode(std::string &buf, std::string &key)
{
    string decoded(buf.length(), 0);
    int i = 0;

    std::transform(buf.begin(), buf.end(), decoded.begin(), 
    [&key, &i](char x) {
        int shift = alpha_to_idx(key[i++]);
        if (i == key.length()) i = 0;
        return shift_left(x, shift);
    });

    return decoded;
}
