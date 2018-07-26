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

std::vector<double> eng_normal_freq = {
	8.12, /* A */
	1.49,
	2.71,
	4.32,
	12.02,
	2.30,
	2.03,
	5.92,
	7.31,
	0.10,
	0.69,
	3.98,
	2.61,
	6.95,
	7.68,
	1.82,
	0.11,
	6.02,
	6.28,
	9.10,
	2.88,
	1.11,
	2.09,
	0.17,
	2.11,
	0.07 /* Z */
};

/*
* Will be used to access eng_normal_freq by index
*/
static inline int letter_to_idx(int letter)
{
	return letter - 'A';
}

static std::string remove_spaces_and_punct(const std::string &buf)
{
	std::string out(buf.size(), 0);
	std::remove_copy_if(buf.begin(), buf.end(), out.begin(),
				[](char c) { return !isalpha(c); });
	return out;
}

static int get_key_len(std::string &out);
static int read_file(const char *filename, std::string &buf);

int main(int argc, char **argv)
{
	std::string     buf;
	std::string		file_name;

	if (argc >= 2) {
		file_name.assign(argv[1]);
	} else {
		file_name.assign(FILENAME);
	}

	if (read_file(file_name.c_str(), buf)) {
		cerr << "Reading failed, exit" << endl;
		return 1;
	}

// remove spaces and punctuation
	std::string out(std::move(remove_spaces_and_punct(buf)));
// convert to uppercase
	std::for_each(out.begin(), out.end(), [](char &ch) { if (isalpha(ch)) ch = toupper(ch); });

	out.resize(strlen(out.c_str()) + 1);
	//cout << out << endl;

	int key_len_1 = get_key_len(out);
	cout << "Key len1: " << key_len_1 << endl;

/*
	Now if key length was guessed correctly, then traversing the text with a step of key length, will produce
	the sequence of chars, which were encrypted by the same letter of the key, and we would try to guess what is 
	it based on the using frequency of each particular English letter
*/
	std::vector<int> traversal;
	std::vector<double> freq_per_shift;
	
	int key_idx = 0;
	for (int key_idx = 0; key_idx < key_len_1; key_idx++) {
		
		int shift = 0;
		/* new traversal on each iteration */
		// ------------------------------------------------------------------
		for (int i = key_idx; i < out.length(); i += key_len_1) {
			traversal.push_back(out[i]);
		}
		freq_per_shift.resize(traversal.size());

		// Calculate the qty of appearing of the each particular letter in the traversal
		std::vector<int> times_matched(26, 0);
		cout << "Traversal len is " << traversal.size() << ", current traversal:";
		for (char ch: traversal) {
			cout << ch; 
			if (isalpha(ch)) {
				//cout << "Char " << x << " appears " << match << " times" << endl;
				times_matched[letter_to_idx(ch)]++;
			}
		}
		cout << endl;

		/* frequencies corresponding to each letter in a traversal */
		std::deque<double> freq_per_traversal(traversal.size(), .0);
		double traversal_size = traversal.size();		
		
		for (int i = 0; i < traversal.size(); i++) {
			char ch = traversal[i];
			freq_per_traversal[i] = times_matched[letter_to_idx(ch)] / traversal_size;
			cout << "freq_per_traversal[" << i << "] = " << freq_per_traversal[i] << endl;
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
		will probably be the shift of corresponding letter in the key against original letter in the text
*/
		do {
			double sum = 0;
			for (int i = 0; i < traversal.size(); i++) {
				char ch = traversal[i];
				sum += freq_per_traversal[i] * (eng_normal_freq[letter_to_idx(ch)] / 100);
			}
			freq_per_shift[shift++] = sum;
			cout << "Shifted " << shift << ", total sum of freq is " << sum << endl;
			/* actual shift: move from the front to the end */
			auto tmp = freq_per_traversal.front();
			freq_per_traversal.pop_front();
			freq_per_traversal.push_back(tmp);
		} while(shift < traversal.size());

/*		for (auto x: freq_per_shift) {
			cout << "shift: freq = " << x << endl; 
		}*/

std::max_element(freq_per_shift.begin(), freq_per_shift.end());

		traversal.clear();
		freq_per_traversal.clear();
		freq_per_shift.clear();
		shift = 0;

		
		break;
	}

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

static int get_key_len(std::string &out)
{
/*
	Guess key length: step 1: on each loop interation, imagine we move the encrypted string by +1 to the right
	against the original one: so char ORIGINAL[0] will correspond now SHIFTED[1] etc 
	A A B X C Z D E
	. A A B X C Z D E
	Compare shifted symbol with the corresponding in the original; look for a coincidences: 
	for instance, only A matches above. The trick is to find some kind of a pattern with the 
	max count of matches, and the interval between most successful matches is probably a key length
*/

	std::vector<int> coincidence;
	int sum = 0;

	for (int shift = 1; shift < out.length() - 1; shift++) {
		for (int i = 0, j = shift; j < out.length() - 1; i++, j++) {
			if (out[i] == out[j]) {
				sum++;
			}
		}
		coincidence.push_back(sum);
		sum = 0;
	}

/* 	int shift = 1;
	for (auto i: coincidence) {
		cout << "shifted by " << shift++ << ": " << i << endl;
		if (shift > 200)
			break;
	} */

	/*
		Guess key length: step 2: compare most successful matches and get the interval between them
	*/
	std::vector<int> freq_factor(13, 0);
	for (int right = 11, left = 0; right <= (out.length() / 2);) {
		/*cout << "Looking for range " << left << " element " << *(coincidence.begin() + left) << " ... " << 
		right << " element " << *(coincidence.begin() + right) << ": ";*/
		auto it = std::max_element(coincidence.begin() + left, coincidence.begin() + right + 1);
		int distance = std::distance(coincidence.begin() + left, it) + 1;
		//cout << "max element is " << *it << " distance is " << distance << endl;
		left += 12;
		right += 12;
		freq_factor[distance]++;
	}

	int idx = 0;
	int key_len_1 = 0, max1 = 0, max2 = 0, key_len_2 = 0;
	for (auto x: freq_factor) {
		if (x > max1) {
			max2 = max1;
			key_len_2 = key_len_1;
			max1 = x;
			key_len_1 = idx;
		}
		cout << idx++ << " : " << x << endl;
	}

	return key_len_1;
}