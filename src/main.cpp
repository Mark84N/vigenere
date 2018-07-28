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

#define EXAMPLE "example"
#define ENCRYPTED "encrypted2"
#define FILENAME ENCRYPTED
#define MAX_KEY_LEN 12 /* known beforehand */

std::vector<double> eng_normal_freq = {
	8.12, /*A*/
	1.49, /*B*/
	2.71, /*C*/
	4.32, /*D*/
	12.02, /*E*/
	2.30, /*F*/
	2.03, /*G*/
	5.92, /*H*/
	7.31, /*I*/
	0.10, /*J*/
	0.69, /*K*/
	3.98, /*L*/
	2.61, /*M*/
	6.95, /*N*/
	7.68, /*O*/
	1.82, /*P*/
	0.11, /*Q*/
	6.02, /*R*/
	6.28, /*S*/
	9.10, /*T*/
	2.88, /*U*/
	1.11, /*V*/
	2.09, /*W*/
	0.17, /*X*/
	2.11, /*Y*/
	0.07 /* Z */
};

/* 
*	Will be used to access eng_normal_freq by index
*/
static inline int letter_to_idx(int letter)
{
	return letter - 'A';
}

static std::string remove_not_letters(const std::string &buf)
{
	std::string out(buf.size(), 0);
	std::remove_copy_if(buf.begin(), buf.end(), out.begin(),
				[](char c) { return !isalpha(c); });
	return out;
}

static int get_key_len(std::string &out);
static int read_file(const char *filename, std::string &buf);
static void freq_analyse1(std::string &buf, int key_len);
static void freq_analyse2(std::string &buf, int key_len);

int main(int argc, char **argv)
{
	std::string     raw_buf;
	std::string		file_name;

	if (argc >= 2) {
		file_name.assign(argv[1]);
	} else {
		file_name.assign(FILENAME);
	}

	if (read_file(file_name.c_str(), raw_buf)) {
		cerr << "Reading failed, exit" << endl;
		return 1;
	}

	std::string buf(std::move(remove_not_letters(raw_buf)));

	if (buf.empty())
		cerr << "Error: Empty file or no alphabet characters present.\n" << endl;

// convert to uppercase
	std::for_each(buf.begin(), buf.end(), [](char &ch) { if (isalpha(ch)) ch = toupper(ch); });

	buf.resize(strlen(buf.c_str()));
	//cout << buf << endl;
	cout << "Buffer: " << buf << endl;

	int key_len = get_key_len(buf);
	cout << "Key len1: " << key_len << endl;
	//cout << buf << endl << endl << endl;
	
	freq_analyse1(buf, key_len);

//	freq_analyse2(buf, key_len_1);


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
/*
	Guess key length: step 1: on each loop interation, imagine we move the encrypted string by +1 to the right
	against the original one: so char ORIGINAL[0] will correspond now SHIFTED[1] etc 
	A A B X C Z D E
	. A A B X C Z D E
	Compare shifted symbol with the corresponding in the original; look for a coincidences: 
	for instance, only A matches above. The trick is to find some kind of a pattern with the 
	max count of matches, and the interval between most successful matches is probably a key length.
*/
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
	cout << "index is " << start_idx << endl;

	/* split matches to 12 parts , to get intervals between most successful ones */
	std::vector<int> intervals_sum(13, 0);
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

static void freq_analyse1(std::string &buf, int key_len)
{
	const char arr[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
	std::vector<int> times_matched(26, 0);
	cout << "------------" << endl;
	cout << "E - A == " << 'E' - 'A' << endl;
	for (int i = 0; i < key_len; i++) {
		for (int j = i; j < buf.length(); j += key_len) {
			times_matched[letter_to_idx(buf[j])]++;
		}
		char ch = 'A';
		for (auto x: times_matched) {
			cout << ch++ << ": " << x << endl;
		}
		cout << "------------" << endl;
		auto it = std::max_element(times_matched.begin(), times_matched.end());
		int idx = std::distance(times_matched.begin(), it);
		char encrypted = 'A' + idx;
		cout << "most frequent encrypted is " << encrypted << " character" << endl;
		
		//char target = letter_to_idx(encrypted) - ('E' - 'A');
		//target = (target < 0? (target += 26) : target) + 'A';
		for (int i = 'E' - 'A'; i > 0; i--) {
			encrypted--;
			if (encrypted < 'A') encrypted = 'Z';
			cout << "step" << endl;
		}
		
		cout << i + 1 << " letter of key is " << encrypted << " (integer " << int(encrypted) << ")" << endl;
		times_matched.assign(26, 0);
	}
}

static void freq_analyse2(std::string &buf, int key_len)
{
	/*
	Now if key length was guessed correctly, then traversing the text with a step of key length, will produce
	the sequence of chars, which were encrypted by the same letter of the key, and we would try to guess what is 
	it
*/
	std::vector<int> traversal;
	std::vector<int> key_shift;

/*
1. get traversal (letters encrypted with the same keys letter);
2. calculate each letter frequency in traversal;
3. multiply frequency of the letter(1) by statistical freq of using in English (2)
4. make a step: shift (1) to the left and multiply by (2), but preserving order from step [3]
*/

	int key_idx = 0;
	for (int key_idx = 0; key_idx < key_len; key_idx++) {
		/*[1]*/
		for (int i = key_idx; i < buf.length(); i += key_len) {
			traversal.push_back(buf[i]);
		}

		/*[2]*/
		std::vector<int> times_matched(26, 0);
		cout << "Traversal len is " << traversal.size() << ", current traversal:";
		for (char ch: traversal) {
			cout << ch; 
			if (isalpha(ch)) {
				times_matched[letter_to_idx(ch)]++;
			}
		}
		cout << endl;

		double traversal_size = traversal.size();
		std::deque<double> freq_per_traversal(traversal_size, .0);
		for (int i = 0; i < traversal_size; i++) {
			char ch = traversal[i];
			freq_per_traversal[i] = times_matched[letter_to_idx(ch)] / traversal_size;
			if (i < 50)
				cout << "freq_per_traversal[" << i << "] = " << freq_per_traversal[i] <<
				" (matched " << times_matched[letter_to_idx(ch)] <<  " times)"<< endl;
		}
		cout << "freq_per_traversal.len() = " << freq_per_traversal.size() << endl;
/*
		Suppose:
		traversal = { A, C, B }; remember - that are all digits from the input text, 
		encoded with the same letter of the key, giving us Caesar cipher;
		freq_per_traversal = { 0.15, 0.25, 0.6 }  -  usage per 1 traversal

		after shift, freq_per_traversal = { 0.25, 0.6, 0.15 };

		multiplying corresponding freq_per_traversal by eng_normal_freq and summing results up, will
		provide us of kinda probability value; comparing those values after all shiftings, the max one
		will probably be the shift of corresponding letter in the key against the original letter in the text
*/
		/*[3]*/
		double sum = 0;
		double max = 0;
		int target_dist = 0;
		int shift = 0;

		do {
			sum = 0;

			for (int i = 0; i < traversal.size(); i++) {
				char ch = traversal[i];
				sum += freq_per_traversal[i] * (eng_normal_freq[letter_to_idx(ch)] / 100);
			}
			if (shift < 50)
				cout << "Shifted " << shift << ", total sum of freq is " << sum << 
				", freq top is " << freq_per_traversal.front() <<
				", freq back is " << freq_per_traversal.back() << endl;

			if (sum > max) {
				max = sum;
				target_dist = shift;
			}
			/* actual shift: move from the front to the end */
			auto tmp = freq_per_traversal.front();
			freq_per_traversal.pop_front();
			freq_per_traversal.push_back(tmp);
			shift++;
		} while (shift < traversal.size());

		cout << "target_dist == " << target_dist << " on max freq of " << max << endl;
		cout << "Thus shift produced by key is most likely " << target_dist % 26 << endl;
		key_shift.push_back(target_dist % 26);

		traversal.clear();
		freq_per_traversal.clear();
		times_matched.clear();
		shift = 0;
		cout << endl << endl;
	}
	cout << "key shift: ";
	for (auto x: key_shift) {
		cout << x << " ";
	}
	cout << endl;
}
