#include "StringSearch.h"
#include <chrono>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::nanoseconds;
using the_clock = std::chrono::steady_clock;

StringSearch::StringSearch()
{
	// Algorithm text output is disabled by default.
	textToggle = false;
}

StringSearch::~StringSearch()
{
}

std::vector<int> StringSearch::searchBoyerMoore(std::string kw, std::string t)
{
	// Assign values to variables.
	keyword = kw;
	text = t;

	keyLength = keyword.length();
	textLength = text.length();

	results.clear();
	occurances = 0;

	// Go through a lookup table setting each element to be false. This lookup table will contain a boolean for each character that is set to true if that character is in the keyword that we are searching for.
	for (int i = 0; i < 256; i++)
	{
		inKeyword[i] = false;
	}

	// For each character in the keyword, change the lookup table's value to be true.
	for (char c: keyword)
	{
		inKeyword[int(c)] = true;
	}

	// If the character's not in the keyword, it can skip the whole word.
	for (int i = 0; i < 256; i++)
	{
		skip[i] = keyLength;
	}
	
	// If the character is in the keyword, it can skip forward the rest of the keyword's length.
	for (int i = 0; i < keyLength; i++)
	{
		skip[int(keyword[i])] = (keyLength - 1) - i;
	}

	// Look for the keyword in the text
	for (int i = 0; i < textLength - keyLength; i++)
	{
		// Set s to the correct value in the skip lookup table.
		int s = skip[int(text[i + keyLength - 1])];

		// If there isn't any matches, skip forwards.
		if (s != 0)
		{
			i += s - 1;
			continue;
		}

		// If there isn't any matches in this part of the text, skip forward.
		else if (!inKeyword[int(text[i + keyLength - 1])])
		{
			i += keyLength - 1;
			continue;
		}

		// Create j outside of the loop since we need to access it after the loop.
		int j;
		for (j = 0; j < keyLength; j++)
		{
			if (text[i + j] != keyword[j]) // If a letter in the keyword doesn't match the text, break the loop
			{
				break;
			}
		}

		if (j == keyLength) // If j got to the end of the loop, the whole keyword was found.
		{
			if (textToggle)
			{
				std::cout << "Found " << keyword << "!\n"; // Output that the keyword was found.
			}
			results.push_back(i); // Add the position in the text to the results vector.
		}
	}

	if (textToggle)
	{
		// Number of occurances of the keyword is equal to the size of the vector.
		occurances = results.size();

		// Display how many times the keyword was found.
		std::cout << "\n'" << keyword << "' was found " << occurances << " time(s).\n\n";

		// Iterate through the vector and display each position that the keyword was found at.
		for (int i = 0; i < occurances; i++)
		{
			std::cout << "Found '" << keyword << "' at position " << results[i] << ".\n";
		}
	}

	return results;
}

std::vector<int> StringSearch::searchRabinKarp(std::string kw, std::string t)
{
	// Assign values to variables.
	keyword = kw;
	text = t;

	keyLength = keyword.length();
	textLength = text.length();

	keyHash = hash(keyword); // The hash of the word we're looking for. If the rolling hash matches this, we might have found the word.
	rollingHash = hash(text.substr(0, keyLength)); // Initial value of the rolling hash

	results.clear();
	occurances = 0;

	// Look for the keyword in the text
	for (int i = 0; i < textLength - keyLength; i++)
	{
		a = hash(text[i]); // hash of the first letter being searched
		b = hash(text[i + keyLength]); // hash of the letter after the last in the rolling hash
		
		//std::cout << keyword << " hash: " << keyHash << ". " << text.substr(i, keyLength) << " hash: " << rollingHash << ".\n";

		// Check if hashes match
		if (rollingHash == keyHash)
		{
			// If the substring matches the keyword, the keyword has been found
			if (text.substr(i, keyLength) == keyword)
			{
				if (textToggle)
				{
					std::cout << "Found " << keyword << "!\n";
				}
				results.push_back(i); // Add the position in the text to the results vector.
			}
		}

		rollingHash = rollingHash - a + b; // Calculate the new value of the rolling hash by subtracting the hash of the first letter and adding the hash of the next letter.
	}

	if (textToggle)
	{
		// Number of occurances of the keyword is equal to the size of the vector.
		occurances = results.size();

		// Display how many times the keyword was found.
		std::cout << "\n'" << keyword << "' was found " << occurances << " time(s).\n\n";

		// Iterate through the vector and display each position that the keyword was found at.
		for (int i = 0; i < occurances; i++)
		{
			std::cout << "Found '" << keyword << "' at position " << results[i] << ".\n";
		}
	}

	return results;
}

int StringSearch::hash(std::string s) // Polynomial rolling hash. Should be O(n) complexity where n is length of the string. When rolling the hash, you're just adding and taking away 1 letter, which is O(1). Implementation of formula 'H = c1a^k-1 + c2a^k-2 + c3a^k-3 ... + cka^0'. 
{
	int h = 0;
	int len = s.length();
	const int a = 1; // a constant value to multiply by
	for (int k = 0; k < len; k++) 
	{
		h += s[k] * pow(a, len - k); // calculating 'to the power of' is resource intensive, but only done once per search as this version of the hash function is only called at the start of the search.
	}
	return h;
}

int StringSearch::hash(char c) // Overloaded version of the rolling hash that just takes a single char value as the argument.
{
	int h = 0;
	const int a = 1; // a constant value to multiply by
	h += c * a; // don't need to calculate to the power of, as putting it to the power of 1 would return the same number.
	//h += c * pow(a, 1);
	return h;
}

void StringSearch::compareDataStructure()
{
	// I narrowed my choices for storing results down to vectors and lists.
	std::vector<int> testVector;
	std::list<int> testList;
	int test = 0;

	// Inform the user about the test.
	std::cout << "\nComparing the performance of vectors and linked lists based on the requirements of the program.\nThe program needs to be able to add the results to the structure, and then be able to retrieve them in the order they were added.\n";

	// Time how long it takes to add integers to the vector, then output it to the console.
	std::cout << "\nAdding integers to the vector...";
	the_clock::time_point startTime = the_clock::now();
	for (int i = 0; i < 1000; i++)
	{
		testVector.push_back(i);
	}
	the_clock::time_point endTime = the_clock::now();

	auto time_taken = duration_cast<nanoseconds>(endTime - startTime).count();
	std::cout << "\nVector took " << time_taken << " nanoseconds to fill with 1000 integers.\n";

	// Time how long it takes to add integers to the list, then output it to the console.
	std::cout << "\nAdding integers to the list...";
	startTime = the_clock::now();
	for (int i = 0; i < 1000; i++)
	{
		testList.push_back(i);
	}
	endTime = the_clock::now();

	time_taken = duration_cast<nanoseconds>(endTime - startTime).count();
	std::cout << "\nList took " << time_taken << " nanoseconds to fill with 1000 integers.\n";

	// Time how long it takes to iterate through the vector, then output it to the console.
	std::cout << "\nIterating through the vector until the final item is reached...";
	startTime = the_clock::now();
	for (int i = 0; i < testVector.size(); i++)
	{
		test = testVector[i];
	}
	endTime = the_clock::now();
	std::cout << "\nFinal item reached: " << test;
	time_taken = duration_cast<nanoseconds>(endTime - startTime).count();
	std::cout << "\nVector took " << time_taken << " nanoseconds to iterate through the 1000 integers in order.\n";

	// Time how long it takes to iterate through the list, then output it to the console.
	std::cout << "\nIterating through the list until the final item is reached...";
	startTime = the_clock::now();
	for (int i = 0; i < testList.size(); i++)
	{
		std::list<int>::iterator it = std::next(testList.begin(), i);
		test = *it;
	}
	endTime = the_clock::now();
	std::cout << "\nFinal item reached: " << test;
	time_taken = duration_cast<nanoseconds>(endTime - startTime).count();
	std::cout << "\nList took " << time_taken << " nanoseconds to iterate through the 1000 integers in order.\n";

	// Output my conclusion from the results.
	std::cout << "\nVectors perform slightly better at filling the structure and much better at accessing the items in order, so I used vectors instead of lists when implementing the algorithms.\n\n";
}
