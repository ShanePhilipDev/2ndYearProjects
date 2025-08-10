// Shane Philip
// 1902474
#include <iostream>
#include <fstream>
#include "StringSearch.h"
#include <chrono>
#include <limits>

// For measuring performance (time).
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using the_clock = std::chrono::steady_clock;

/* 
Algorithms compared:
Boyer-Moore
and
Rabin-Karp
*/

// Loads in a text file and assigns it to a string.
void loadTextFile(std::string filename, std::string& target)
{
	std::ifstream ifs(filename);
	target.assign(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);
}

// Function to ensure that the program doesn't fail if an invalid input is received.
void validateInput()
{
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input.\n";
	}
}

int main()
{
	// Initialise time measurement variables.
	the_clock::time_point startTime = the_clock::now();
	the_clock::time_point endTime = the_clock::now();
	auto time_taken = duration_cast<milliseconds>(endTime - startTime).count();

	// An object from my string search class which I used to implement the algorithms.
	StringSearch stringSearcher;

	// Creating and assigning the string objects that the string search algorithms are going to search through.
	std::string smallText, mediumText, largeText;
	smallText = "How much wood would a woodchuck chuck if a woodchuck could chuck wood?";
	loadTextFile("rickroll.txt", mediumText);
	loadTextFile("Shrek.txt", largeText);

	// Used for storing the results of the string search, and storing them in a csv file.
	std::ofstream resultsFile("results.csv");
	std::vector<int> results;

	// Integers for holding the user's input.
	int x = 0;
	int y = 0;
	
	// Do while loop... Repeats until the user decides to exit.
	do 
	{
		// Displays the options that the user has to choose from.
		std::cout << "Enter 1 to test Boyer-Moore algorithm.\nEnter 2 to test Rabin-Karp algorithm.\nEnter 3 to compare the vector and linked list data structures.\nEnter 4 to toggle text output while running (disabled by default).\nEnter 5 to exit.\nPlease enter a number: ";
		std::cin >> x; // recieve user input
		validateInput();
		switch (x)
		{
		case 1:
			// Ask user how many times they wish to run the algorithm and receive their input.
			std::cout << "\n\nHow many times would you like to run the Boyer-Moore algorithm?\n";
			std::cin >> y;
			validateInput();
			break;
		case 2:
			// Ask user how many times they wish to run the algorithm and receive their input.
			std::cout << "\n\nHow many times would you like to run the Rabin-Karp algorithm?\n";
			std::cin >> y;
			validateInput();
			break;
		case 3:
			// Calls function to compare the data structures that I considered when creating the algorithms.
			stringSearcher.compareDataStructure();
			break;
		case 4:
			// Turns text output while the algorithm is runnning on or off. It is off by default as it has a huge impact on performance.
			stringSearcher.setOutputText(!stringSearcher.getOutputText());
			std::cout << "\n";
		default:
			std::cout << "\n";
			break;
		}

		// If the user chose to test the Boyer-Moore algorithm...
		if (x == 1)
		{
			// Run the algorithm once to retrieve the results.
			std::cout << "\nShort length text: Searching for how many occurances of 'wood' are in the tongue-twister 'How much wood would a woodchuck chuck if a woodchuck could chuck wood?'\n";
			results = stringSearcher.searchBoyerMoore("wood", smallText);

			// Add the results to the results file.
			resultsFile << "Boyer-Moore Algorithm\n\nWord, Position\n";
			for (int i = 0; i < results.size(); i++)
			{
				resultsFile << "'wood'," << results[i] << "\n";
			}
			resultsFile << "Occurances:," << results.size() << "\n";

			// Measure the performance of the algorithm.
			// Gets the time it takes to run the algorithm y amount of times, then adds it to the results file and outputs it to the console.
			startTime = the_clock::now();
			for (int i = 0; i < y; i++)
			{
				stringSearcher.searchBoyerMoore("wood", smallText);
			}
			endTime = the_clock::now();
			time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			resultsFile << "Time taken to run " << y << " times:," << time_taken << ",ms\n\n";
			std::cout << "Time taken to run " << y << " times: " << time_taken << "ms\n\n";

			// Repeat this process for the medium length text to see how performance varies based on the text's size.
			std::cout << "Medium length text: Searching for how many occurances of 'Never gonna' are in the song 'Never Gonnna Give You Up' by Rick Astley.\n";
			results = stringSearcher.searchBoyerMoore("Never gonna", mediumText);

			resultsFile << "Word, Position\n";
			for (int i = 0; i < results.size(); i++)
			{
				resultsFile << "'Never gonna'," << results[i] << "\n";
			}
			resultsFile << "Occurances:," << results.size() << "\n";

			startTime = the_clock::now();
			for (int i = 0; i < y; i++)
			{
				stringSearcher.searchBoyerMoore("Never gonna", mediumText);
			}
			endTime = the_clock::now();
			time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			resultsFile << "Time taken to run " << y << " times:," << time_taken << ",ms\n\n";
			std::cout << "Time taken to run " << y << " times: " << time_taken << "ms\n\n";

			// Repeat again for the longest text.
			std::cout << "Long length text: Searching for how many occurances of 'Shrek' are in the script of the movie 'Shrek'.\n";
			results = stringSearcher.searchBoyerMoore("Shrek", largeText);

			resultsFile << "Word, Position\n";
			for (int i = 0; i < results.size(); i++)
			{
				resultsFile << "'Shrek'," << results[i] << "\n";
			}
			resultsFile << "Occurances:," << results.size() << "\n";

			startTime = the_clock::now();
			for (int i = 0; i < y; i++)
			{
				stringSearcher.searchBoyerMoore("Shrek", largeText);
			}
			endTime = the_clock::now();
			time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			resultsFile << "Time taken to run " << y << " times:," << time_taken << ",ms\n\n";
			std::cout << "Time taken to run " << y << " times: " << time_taken << "ms\n\n";
		}
		else if (x == 2) // If the user chose to test the Rabin-Karp algorithm...
		{
			// Run the algorithm once to retrieve the results.
			std::cout << "\nShort length text: Searching for how many occurances of 'wood' are in the tongue-twister 'How much wood would a woodchuck chuck if a woodchuck could chuck wood?'\n";
			results = stringSearcher.searchRabinKarp("wood", smallText);

			// Add the results to the results file.
			resultsFile << "Rabin-Karp Algorithm\n\nWord, Position\n";
			for (int i = 0; i < results.size(); i++)
			{
				resultsFile << "'wood'," << results[i] << "\n";
			}
			resultsFile << "Occurances:," << results.size() << "\n";

			// Measure the performance of the algorithm.
			// Gets the time it takes to run the algorithm y amount of times, then adds it to the results file and outputs it to the console.
			startTime = the_clock::now();
			for (int i = 0; i < y; i++)
			{
				stringSearcher.searchRabinKarp("wood", smallText);
			}
			endTime = the_clock::now();
			time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			resultsFile << "Time taken to run " << y << " times:," << time_taken << ",ms\n\n";
			std::cout << "Time taken to run " << y << " times: " << time_taken << "ms\n\n";

			// Repeat this process for the medium length text to see how performance varies based on the text's size.
			std::cout << "Medium length text: Searching for how many occurances of 'Never gonna' are in the song 'Never Gonnna Give You Up' by Rick Astley.\n";
			results = stringSearcher.searchRabinKarp("Never gonna", mediumText);

			resultsFile << "Word, Position\n";
			for (int i = 0; i < results.size(); i++)
			{
				resultsFile << "'Never gonna'," << results[i] << "\n";
			}
			resultsFile << "Occurances:," << results.size() << "\n";

			startTime = the_clock::now();
			for (int i = 0; i < y; i++)
			{
				stringSearcher.searchRabinKarp("Never gonna", mediumText);
			}
			endTime = the_clock::now();
			time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			resultsFile << "Time taken to run " << y << " times:," << time_taken << ",ms\n\n";
			std::cout << "Time taken to run " << y << " times: " << time_taken << "ms\n\n";

			// Repeat again for the longest text.
			std::cout << "Long length text: Searching for how many occurances of 'Shrek' are in the script of the movie 'Shrek'.\n";
			results = stringSearcher.searchRabinKarp("Shrek", largeText);

			resultsFile << "Word, Position\n";
			for (int i = 0; i < results.size(); i++)
			{
				resultsFile << "'Shrek'," << results[i] << "\n";
			}
			resultsFile << "Occurances:," << results.size() << "\n";

			startTime = the_clock::now();
			for (int i = 0; i < y; i++)
			{
				stringSearcher.searchRabinKarp("Shrek", largeText);
			}
			endTime = the_clock::now();
			time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			resultsFile << "Time taken to run " << y << " times:," << time_taken << ",ms\n\n";
			std::cout << "Time taken to run " << y << " times: " << time_taken << "ms\n\n";
		}

	} while (x != 5);
	return 0;
}