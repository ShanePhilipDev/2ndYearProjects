#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <cmath>

// This class contains both the Boyer-Moore and Rabin-Karp algorithms.
class StringSearch
{
public:
	// Constructor and destructor.
	StringSearch();
	~StringSearch();

	// Functions to be called when you want to run the algorithms.
	std::vector<int> searchBoyerMoore(std::string kw, std::string t);
	std::vector<int> searchRabinKarp(std::string kw, std::string t);

	// Hashing algorithms for hashing a specified string or char. Only used in the Rabin-Karp algorithm.
	int hash(std::string s);
	int hash(char c);

	// A function to show the performance differences between lists and vectors in this application.
	void compareDataStructure();

	// For toggling whether text should be outputted to the console while the algorithms are running.
	void setOutputText(bool b) { textToggle = b; };
	bool getOutputText() { return textToggle; };

protected:
	// Strings to store the word or phrase being searched for, and the text being searched through.
	// The integers store their length.
	std::string keyword;
	std::string text;
	int keyLength;
	int textLength;

	// Hash variables used in the Rabin-Karp algorithm.
	int keyHash;
	int rollingHash;

	// Used when calculating the rolling hash.
	int a;
	int b;

	// LookUp tables used in the Boyer-Moore algorithm. Arrays were used as they have a fixed size and we are only interested in looking at 256 characters. Using a fixed-size structure also means we know exactly how much memory has been allocated to it and each element in the array will be next to each other in memory allowing for quicker access.
	bool inKeyword[256];
	int skip[256];

	// Boolean to hold whether text should be outputted.
	bool textToggle;

	// Stores the position of each occurance of the keyword in the text. I chose this over a list because it is faster at pushing back objects and is much faster at iterating through objects. It is also easier to access each element, as you just have to write 'results[i]' rather than creating an iterator pointer and using that to go through each point in the structure.
	std::vector<int> results; 

	// Stores the number of occurances of the keyword in the text.
	int occurances;
	
};

