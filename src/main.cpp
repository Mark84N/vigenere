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

std::vector<double> letter_freq = {
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
* Will be used to access letter_freq by index
*/
static inline int letter_to_idx(int letter)
{
	return letter - 'A';
}

static std::string remove_spaces_and_punct(const std::string &buf)
{
	std::string out(buf.size(), 0);
	std::remove_copy_if(buf.begin(), buf.end(), out.begin(),
				[](char c) { return (ispunct(c) || isspace(c)); });
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

// remove whitespaces and punctuation + 
	std::string out(std::move(remove_spaces_and_punct(buf)));
// convert to uppercase
	std::for_each(out.begin(), out.end(), [](char &ch) { if (isalpha(ch)) ch = toupper(ch); });

	//cout << out << endl;
	out.resize(strlen(out.c_str()) + 1);
	cout << out << endl;

	int key_len_1 = get_key_len(out);
	cout << "Key len1: " << key_len_1 << endl;

/*
	Now if key length was guessed correctly, then traversing the text with a step of key length, will produce
	the sequence of chars, which were encrypted by the 1st letter of the key, and we would try to guess what is 
	it based on the frequency of using each particular letter in English
*/
	std::deque<int> traversal;
	for (int i = 0; i < out.length(); i += key_len_1) {
		traversal.push_back(out[i]);
	}

	// Calculate the qty of appearing of the each particular letter in the traversal
	std::vector<int> appears(26, 0);
	cout << "Traversal len is " << traversal.size();
	for (char ch: traversal) {
		if (isalpha(ch)) {
			//cout << "Char " << x << " appears " << match << " times" << endl;
			appears[letter_to_idx(ch)]++;
		}
	}
	cout << endl;


	int j = 0;
	do {
		char curr = 'A';
		std::vector<double> appear_freq(26, .0);
		for (int times_matched: appears) {
			int idx = letter_to_idx(curr);
			appear_freq[idx] = ((double)(times_matched) / (double)traversal.size());
				
			//cout << "1 Character \"" << curr << "\"" << " appears " << times_matched << " times, statically it is " << appear_freq[idx] << endl;

			appear_freq[idx] *= letter_freq[idx];

			//cout << "2 Character \"" << curr << "\"" << " appears " << times_matched << " times, statically it is " << appear_freq[idx] << endl;
			curr++;
		}

		double freq = .0;
		std::for_each(appear_freq.begin(), appear_freq.end(), [&freq](double f) { freq += f; });
		cout << "For shift " << j << ", freq sum is " << freq << endl;
		// shift to the left
		//int tmp = traversal.front();
		//traversal.pop_front();
		//traversal.push_back(j);
		j++;
	} while(j < traversal.size());
	/* Keep aligning traversal results with a shift to the left size to get the largest possible valuet */


	cout << endl;

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