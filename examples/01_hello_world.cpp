/* 01_hello_world.cpp
 * 
 * C++ version of the classic "Hello World!" program, for reference.
 * During the course we will write mostly C-style code, with a few C++ features when appropriate
 * (NB: Unless discussing specifically differences between the two, I will probably
 *  use C and C++ interchangeably)
 * 
 * author: chris
 */

// include directive - load functionalities from libraries
// this include loads C++ standard Input/Output stream functionalities
#include <iostream>

// `main` => entry point of your program
// 1 and only 1 allowed per executable 
// returns an integer variable and takes no parameters (void is not necessary)
int main(void)
{
	// puts a string on the standard output stream
	// 'std::endl' is a platform-independent newline character
	std::cout << "Hello World!" << std::endl;

	// common C convention: an entire program returning 0 means "execution terminating correctly"
	// non-zero returns would imply an error occured
	return 0;
}
