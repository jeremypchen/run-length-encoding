// rle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rletest.h"
#include "rle-algo.hpp"
#include <iostream>
#include "rle-files.h"
#include <sstream>

void subMenu2();
void subMenu3();
void mainMenu();

void Part1Tests()
{
	std::cout << "Testing compression..." << std::endl;
	RLE<char> r;
	r.Compress(CHAR_1, strlen(CHAR_1));
	std::cout << r << std::endl;
	r.Compress(CHAR_2, strlen(CHAR_2));
	std::cout << r << std::endl;
	r.Compress(CHAR_3, strlen(CHAR_3));
	std::cout << r << std::endl;
	r.Compress(CHAR_4, strlen(CHAR_4));
	std::cout << r << std::endl << std::endl;

	std::cout << "Testing decompression..." << std::endl;
	r.Decompress(RLE_CHAR_1, strlen(RLE_CHAR_1), 40);
	std::cout << r << std::endl;
	r.Decompress(RLE_CHAR_2, strlen(RLE_CHAR_2), 12);
	std::cout << r << std::endl;
	r.Decompress(RLE_CHAR_3, strlen(RLE_CHAR_3), 14);
	std::cout << r << std::endl << std::endl;
}

// MAIN MENU
void mainMenu(){
	// Main entry point!
	bool validEntry = false;
	int intInput;

	while (!validEntry){ // loop in case entry was invald
		std::cout << "Select an option:" << std::endl;
		std::cout << "1. Part 1 (Tests)" << std::endl;
		std::cout << "2. Part 2 (Single Files)" << std::endl;
		std::cout << "3. Part 3 (Directories)" << std::endl;
		std::cout << "4. Exit" << std::endl;

		std::string stringInput;
		std::getline(std::cin, stringInput);

		std::stringstream(stringInput) >> intInput;

		if (intInput == 1 || intInput == 2 || intInput == 3 || intInput == 4){
			validEntry = true;
		}
		else {
			std::cout << "Not a valid menu choice." << std::endl << std::endl;
		}
	}

	switch (intInput){
	case (1) :
		Part1Tests();
		mainMenu();
		break;
	case (2) :
		subMenu2();
		break;
	case (3) :
		subMenu3();
		break;
	case (4) :
		std::cout << "Exiting program." << std::endl;
		break;
	default:
		break;
	}
}

// SUBMENU 2 (Single files)
void subMenu2(){
	bool validEntry = false;
	int intInput;

	while (!validEntry){
		std::cout << "Select an option:" << std::endl;
		std::cout << "1. Create an archive" << std::endl;
		std::cout << "2. Extract an archive" << std::endl;
		std::cout << "3. Return to previous menu" << std::endl;

		std::string stringInput;
		std::getline(std::cin, stringInput);

		std::stringstream(stringInput) >> intInput;

		if (intInput == 1 || intInput == 2 || intInput == 3){
			validEntry = true;
		}
		else {
			std::cout << "Not a valid menu choice." << std::endl << std::endl;
		}
	}

	std::string filename;
	if (intInput == 1 || intInput == 2){
		std::cout << "Enter the file name of the archive." << std::endl;

		std::getline(std::cin, filename);

	}

	RLE_v1 r;

	switch (intInput){
	case (1) :
		r.CreateArchive(filename);
		subMenu2();
		break;
	case (2) :
		r.ExtractArchive(filename);
		subMenu2();
		break;
	case (3) :
		mainMenu();
		break;
	default:
		break;
	}
}

// SUBMENU 3 (Directories)
void subMenu3(){
	bool validEntry = false;
	int intInput;

	while (!validEntry){
		std::cout << "Select an option:" << std::endl;
		std::cout << "1. Create an archive" << std::endl;
		std::cout << "2. Extract an archive" << std::endl;
		std::cout << "3. Return to previous menu" << std::endl;

		std::string stringInput;
		std::getline(std::cin, stringInput);

		std::stringstream(stringInput) >> intInput;

		if (intInput == 1 || intInput == 2 || intInput == 3){
			validEntry = true;
		}
		else {
			std::cout << "Not a valid menu choice." << std::endl << std::endl;
		}
	}

	std::string input;

	if (intInput == 1){
		std::cout << "Enter the directory name of the archive." << std::endl;

		std::getline(std::cin, input);

	}
	else if (intInput == 2){
		std::cout << "Enter the file name of the archive." << std::endl;

		std::getline(std::cin, input);

	}

	RLE_v2 r;

	switch (intInput){
	case (1) :
		r.CreateArchive(input);
		subMenu3();
		break;
	case (2) :
		r.ExtractArchive(input);
		subMenu3();
		break;
	case (3) :
		mainMenu();
		break;
	default:
		break;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	mainMenu();
	return 0;
}

