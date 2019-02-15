/* @file: basic2.cpp
 *
 * This script allows finding all the english words in the file:
 * @see brit-a-z.txt
 * 
 * Words are looked for in a 4x4 array of characters, provided a continuity 
 * of the characters in one of the 8 boxes directly in contact with the character.
 * 
 * Input into the script the 4x4 characters,and it outputs all the matching words.
 * @Example: For the 4x4 list of characters:
 * 				A M A F
 * 				B E A R
 * 				T A P U
 * 				G N I S
 * 			 Input the string: amafbeartapugnqis
 *
 * By Thabet Marwa. Jan-2014.
 * Modified: July-2017.
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
using namespace std;

// Declare Class Point
struct Point {
public:
	Point() : x(0), y(0) {};
	Point(int a, int b) : x(a), y(b) {};
	int x;
	int y;
	bool operator!=(const Point& src) {return ((src.x != x) && (src.y != y)); }
	bool operator==(const Point& src) {return ((src.x == x) && (src.y == y)); }
};
Point operator+(const Point& a,  const Point& b) {return Point(a.x+b.x, a.y+b.y); }
Point operator-(const Point& a, const Point& b) {return Point(a.x-b.x, a.y-b.y); }

// Declare Global Variables
vector<string> englishwords(0, ""); // List of All english words
vector<string> wordslist; // List of the Words that exist in the Table
char Table2D[4][4];

// List of all possible directions allowable inside the game
const Point direction[8] = {Point(1,0),Point(1,1), Point(0,1), Point(-1,1), Point(-1,0) , Point(-1,-1), Point(0,-1), Point(1,-1)};	
const Point origin;

// Function Declarations
bool findWord(string word);
bool findWordStart(string word);
bool findWordStart(queue<Point> a, Point trial);
bool inTable(Point a);

int main(int argc, char const *argv[])
{
	// Load Dictionary
	ifstream source("brit-a-z.txt");
	if (source.fail()) {
		cout << "Unable to read source." << endl;
		cin.ignore();
		return 0;
	}

	// Load the words in the dictionary
	string word = "";
	while (!source.eof()) {
		source >> word;
		englishwords.push_back(word);
	}
	cout << "English words count: " << englishwords.size() << endl;

	if (argc > 1)
	{
		word = argv[1];
	} else {
		// Input the characters from the user
		cout << "\nInput the set of characters that you want to be matched. \n";
		cin >> word;
		while (word.length() != 16) {
			cout << "The word length is " << word.length() << ", but it should be 16. Please try again.\n";
			cin >> word;
		}
	}
	cout << "Using the character set " << word << "\n";

	// Fill in the characters into the 2D Table
	for (int i = 0; i < 4; i++) 
		for (int j = 0; j < 4; j++) 
			Table2D[i][j] = word[4*i+j];

	int attemptcount = 0;
	for (int i = 0 ; i < 4; i++) {
		for (int j = 0 ; j < 4; j++) {
			Point seed(i,j); 

			// A stack of the words, where each word is a queue of points
			stack< queue<Point> > mylist;
	
			// Create the first 'word' queue
			queue<Point> seedword;
			seedword.push(seed); // Add to it the first seed Point
			mylist.push(seedword); // Add the word to mylist

			while (!mylist.empty()) {
				// Get a word from the stack mylist
				seedword = mylist.top();
				mylist.pop();

				// For each possible direction from the last point in the word
				for (int k = 0; k < 8; k++) {
					attemptcount++;
					Point mypoint = seedword.back()+direction[k];
					
					if (inTable(mypoint) && findWordStart(seedword,mypoint)) {
						// If the queue list makes a start of a word, then
						queue<Point> q = seedword; // Copy the original list
						q.push(mypoint); // Add to the new list the last character
						mylist.push(q); // Add the last character to mylist
					}
				}
			}
		}
	}

	// Sort the wordslist in order of increasing size of string
	queue<string> radixsort[10];

	int max = 0;	
	for (int i = 0; i < wordslist.size(); i++)
		if (wordslist[i].length() > max) max = wordslist[i].length();

	int digitcount = 1;
	while (max / 10) digitcount++;

	for (int k = 1; k <= digitcount; k*=10) {
		while (!wordslist.empty()) {
			string myword = wordslist.back();
			wordslist.pop_back();
			int kthdigit = (myword.length() / k) % 10;			
			radixsort[kthdigit].push(myword);
		}
		for (int i = 0; i < 10; i++) {
			while(!radixsort[i].empty()) {
				wordslist.push_back(radixsort[i].front());
				radixsort[i].pop();
			}
		}
	}

	cout << "Words List is: " << endl;
	for (int i = 0; i < wordslist.size(); i++) {
		cout << wordslist[i] << endl;
	}
	cout << "\nWord Count = " << wordslist.size() <<
		"\nNumber of Attempts = " << attemptcount << endl;
	cin.ignore();
}

// Checks if the given word is available in the dictionary. O(log(n))
bool findWord(string word) {
	int small = 0, large = englishwords.size() - 1, wordindex;
	while (large - small > 1) {
		wordindex = (small + large)/2;		
		if (englishwords[wordindex] > word) large = wordindex;
		else if (englishwords[wordindex] < word) small = wordindex;
		else	return true;
	}
	if (englishwords[large] == word || englishwords[small] == word) return true;

	return false;
}

bool findWordStart(string word) {
	int small = 0, large = englishwords.size()-1, wordindex;
	bool found = false;
	string key;
	while (large - small > 1) {
		wordindex = (small + large)/2;
		key = englishwords[wordindex];
		while (key.length() > word.length()) key.pop_back();
		if (key > word) large = wordindex;
		else if (key < word) small = wordindex;
		else {
			if (findWord(word)) {
				for (int i = 0; i < wordslist.size(); i++)
					if (wordslist[i] == word) return true;

				wordslist.push_back(word);
			}
			return true;
		}
	}
	if (englishwords[large] == word || englishwords[small] == word) {
		if (findWord(word)) wordslist.push_back(word);
		return true;
	}
	return found;
}

bool findWordStart(queue<Point> a, Point trial) {
	if (a.size() == 0) return true;
	string word = "";
	while (!a.empty()) {
		Point mypoint = a.front();
		a.pop();
		if (mypoint == trial) return false; // Would not allow for the points to overwrite on themselves
		word.push_back(Table2D[mypoint.x][mypoint.y]);
	}
	word.push_back(Table2D[trial.x][trial.y]);
	return findWordStart(word);
}

bool inTable(Point a) {
	return (a.x > -1 && a.x < 4 && a.y > -1 && a.y < 4);
}
