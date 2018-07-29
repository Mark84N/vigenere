#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <algorithm>
#include <cstring>
#include <map>
#include <vector>
#include <deque>

using std::cout;
using std::cerr;
using std::endl;

#define MAX_KEY_LEN 12 /* known beforehand */

std::vector<double> eng_normal_freq = {
	8.12 / 100, /*A*/
	1.49 / 100, /*B*/
	2.71 / 100, /*C*/
	4.32 / 100, /*D*/
	12.02 / 100, /*E*/
	2.30 / 100, /*F*/
	2.03 / 100, /*G*/
	5.92 / 100, /*H*/
	7.31 / 100, /*I*/
	0.10 / 100, /*J*/
	0.69 / 100, /*K*/
	3.98 / 100, /*L*/
	2.61 / 100, /*M*/
	6.95 / 100, /*N*/
	7.68 / 100, /*O*/
	1.82 / 100, /*P*/
	0.11 / 100, /*Q*/
	6.02 / 100, /*R*/
	6.28 / 100, /*S*/
	9.10 / 100, /*T*/
	2.88 / 100, /*U*/
	1.11 / 100, /*V*/
	2.09 / 100, /*W*/
	0.17 / 100, /*X*/
	2.11 / 100, /*Y*/
	0.07 / 100 /* Z */
};

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
									(ch_idx + shift) % 25);
}

static std::string remove_not_alpha(const std::string &buf)
{
	std::string out(buf.size(), 0);
	std::remove_copy_if(buf.begin(), buf.end(), out.begin(),
				[](char c) { return !isalpha(c); });
	return out;
}

static int get_key_len(std::string &out);
static int read_file(const char *filename, std::string &buf);
static std::string freq_analyse1(std::string &buf, int key_len);
static std::string freq_analyse2(std::string &buf, int key_len);

int main(int argc, char **argv)
{
	std::string     raw_buf;
	std::string		file_name;

	if (argc == 2) {
		file_name.assign(argv[1]);
	} else {
		cerr << "Incorrect number of arguments, terminate." << endl;
		return 1;
	}

	if (read_file(file_name.c_str(), raw_buf)) {
		cerr << "Reading failed, terminate." << endl;
		return 1;
	}

	std::string buf(std::move(remove_not_alpha(raw_buf)));

	if (buf.empty()) {
		cerr << "Empty file supplied or no alphabet characters present, terminate." << endl;
		return 1;
	}

// convert to uppercase
	std::for_each(buf.begin(), buf.end(), [](char &ch) { if (isalpha(ch)) ch = toupper(ch); });
	buf.resize(strlen(buf.c_str()));

	int key_len = get_key_len(buf);
	cout << "Key len: " << key_len << endl;

	std::string key = freq_analyse1(buf, key_len);
	cout << "KEY, algo 1 " << key << endl;

	std::string res = freq_analyse2(buf, key_len);
	cout << "KEY, algo 2 " << res << endl;

	return 0;
}

static int read_file(const char *filename, std::string &buf)
{
	std::ifstream   file;
	std::streampos  file_length;

	file.open(filename, std::ios::in | std::ios::ate);
	if (!file.is_open()) {
		cerr << "Failed to open file \"" << filename << "\"" << endl;
		return 1;
	}

	file_length = file.tellg();
	file.seekg(0, std::ios_base::beg);
	file.clear();

	buf.resize(file_length);
	file.read(&buf[0], file_length);
	file.close();

	return 0;
}

static int get_key_len(std::string &buf)
{
	std::vector<int> match_per_shift;
	int sum = 0;

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

static std::string freq_analyse1(std::string &buf, int key_len)
{
	std::vector<int> times_matched(25, 0);
	std::string key;

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

static std::string freq_analyse2(std::string &buf, int key_len)
{
/*
	Now if key length was guessed correctly, then traversing the text with a step of key 
	length [1], will produce the sequence of chars, which were encrypted with the same letter
	of the key;
*/
	std::vector<int> traversal;
	std::string key;

	int key_idx = 0;
	for (int key_idx = 0; key_idx < key_len; key_idx++) {
		/*[1]*/
		for (int i = key_idx; i < buf.length(); i += key_len) {
			traversal.push_back(buf[i]);
		}

		/*[2]*/
		std::vector<int> times_matched(25, 0);
		for (char ch: traversal) {
			if (isalpha(ch)) {
				times_matched[alpha_to_idx(ch)]++;
			}
		}

		std::deque<double> freq(25, 0);
		int total_size = traversal.size();
		for (int i = 0; i < 26; i++) {
			freq[i] = (times_matched[i] / double(total_size));
		}

		double sum = .0, max = .0;
		int shift = 0;
		/* for each shift of freq against alphabet */
		for (int i = 0; i < 26; i++) {
			sum = .0;
			/* for each symbol in alphabet */
			for (int j = 0; j < 26; j++)
				sum += freq[j] * eng_normal_freq[j];

			if (sum > max) {
				max = sum;
				shift = i;
			}

			/* do the shift */
			auto tmp = freq.front();
			freq.pop_front();
			freq.push_back(tmp);
		}

		key.push_back('A' + shift);
		traversal.clear();
	}

	return key;
}
