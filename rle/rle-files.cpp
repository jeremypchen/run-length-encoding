#include "rle-files.h"
#include "rle-algo.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <windows.h>

// Create Archive for Single File
void RLE_v1::CreateArchive(const std::string& source)
{
	std::ifstream::pos_type size;
	char* memblock;

	std::ifstream file(source, std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[static_cast<unsigned int>(size)];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();
		// File data has now been loaded into memblock array

		m_Data.Compress(memblock, static_cast<int>(size)); // Compress function, stored in m_Data.m_Data

		double sizeOfOriginal = static_cast<double>(size);
		double sizeOfCompressed = static_cast<double>(m_Data.m_Size);

		std::cout << std::endl << 100 - static_cast<int>(100 * (sizeOfCompressed / sizeOfOriginal)) << "% compression achieved." << std::endl;

		std::string outputFilename;
		outputFilename = source + ".rl1";

		// Header information
		m_Header.sig[0] = 'R';
		m_Header.sig[1] = 'L';
		m_Header.sig[2] = 'E';
		m_Header.sig[3] = 0x01;
		m_Header.fileSize = static_cast<int>(sizeOfOriginal);
		m_Header.fileNameLength = source.length();
		m_Header.fileName = source;

		std::ofstream arc(outputFilename, std::ios::out | std::ios::binary | std::ios::trunc);
		if (arc.is_open())
		{
			// First write the header data
			arc.write(m_Header.sig, 4);
			arc.write(reinterpret_cast<char*>(&(m_Header.fileSize)), 4);
			arc.write(reinterpret_cast<char*>(&(m_Header.fileNameLength)), 1);
			arc.write(m_Header.fileName.c_str(), m_Header.fileNameLength);
			// Then write the data
			arc.write(m_Data.m_Data, m_Data.m_Size);
		}

		std::cout << "Compressed file created." << std::endl << std::endl;

		// Make sure to clean up!
		delete[] memblock;
	}
	else {
		std::cout << "File not found." << std::endl << std::endl;
	}
}

void RLE_v1::ExtractArchive(const std::string& source)
{
	std::ifstream::pos_type size;
	char* memblock;

	std::ifstream file(source, std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[static_cast<unsigned int>(size)];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();
	}
	else {
		std::cout << "File not found." << std::endl << std::endl;
		return;
	}

	const char* temp = &(memblock[0]); // point the first element

	// First four elements will be the header signature
	m_Header.sig[0] = *temp;
	m_Header.sig[1] = *(++temp);
	m_Header.sig[2] = *(++temp);
	m_Header.sig[3] = *(++temp);

	// Check that it's an RLE1 file
	if (m_Header.sig[0] != 'R' || m_Header.sig[1] != 'L' || m_Header.sig[2] != 'E' || m_Header.sig[3] != 0x01){
		std::cout << "Not a valid file to extract (must be RLE1)." << std::endl << std::endl;
		return;
	}
	else {
		// Next elements are file size and fileNameLength
		m_Header.fileSize = *(reinterpret_cast<int*>(&memblock[4]));
		m_Header.fileNameLength = *(reinterpret_cast<int*>(&memblock[8])); // 4 bits apart because fileSize is an int

		temp = &memblock[9];

		for (int i = 0; i < m_Header.fileNameLength; i++){ // Grab as many characters as filenamelength
			m_Header.fileName += *temp;
			temp++;
		}

		int sizeOfData = static_cast<int>(size)-9 - m_Header.fileNameLength; // Size of Data takes the whole size of the file minus header information
		char * data = new char[sizeOfData];

		for (int i = 0; i < sizeOfData; i++){
			data[i] = *(&memblock[9 + m_Header.fileNameLength + i]); // Grab data at each spot and store in buffer
		}

		m_Data.Decompress(data, sizeOfData, m_Header.fileSize);

		std::ofstream arc(m_Header.fileName, std::ios::out | std::ios::binary | std::ios::trunc);
		if (arc.is_open())
		{
			arc.write(m_Data.m_Data, m_Header.fileSize); // Write out the data in new file
		}

		std::cout << "File successfully extracted as " << m_Header.fileName << std::endl << std::endl;

		// Make sure to clean up!
		delete[] memblock;
	}
}

void RLE_v2::CreateArchive(const std::string& source)
{
	// Find the first file in the directory.
	WIN32_FIND_DATA ffd;
	std::string path = source + "\\*.*";
	HANDLE hFind = FindFirstFile(path.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		// No files found, display error message and return!
		std::cout << "Could not find given file." << std::endl << std::endl;
		return;
	}

	// Add all files in this directory to a list of strings
	std::list<std::string> files;
	do
	{
		std::string temp = ffd.cFileName;
		if (temp != "." && temp != ".." &&
			!(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
		{
			files.push_back(ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	for (std::string fileString : files){ // Go through each file and...
		std::string directoryAndFileName = source + "/" + fileString;
		std::ifstream file(directoryAndFileName, std::ios::in | std::ios::binary | std::ios::ate);

		if (file.is_open())
		{
			std::ifstream::pos_type size;
			char* memblock;

			file_info *fileInfo = new file_info(); // Create new file info 

			m_Files.push_back(fileInfo); // Store it

			size = file.tellg();
			memblock = new char[static_cast<unsigned int>(size)];
			file.seekg(0, std::ios::beg);
			file.read(memblock, size);
			file.close();
			// File data has now been loaded into memblock array

			fileInfo->data.Compress(memblock, static_cast<int>(size)); // Compress the file data

			// Set file_info information
			fileInfo->fileSize = static_cast<int>(size); 
			fileInfo->compressedSize = fileInfo->data.m_Size;
			fileInfo->fileNameLength = fileString.length();
			fileInfo->fileName = fileString;
		}
	}

	// Create header info for directory
	m_Header.sig[0] = 'R';
	m_Header.sig[1] = 'L';
	m_Header.sig[2] = 'E';
	m_Header.sig[3] = 0x02;
	m_Header.dirNameLength = source.length();
	m_Header.dirName = source;

	std::string outputFilename;
	outputFilename = source + ".rl2";

	std::ofstream arc(outputFilename, std::ios::out | std::ios::binary | std::ios::trunc);
	if (arc.is_open())
	{
		// First write header information
		arc.write(m_Header.sig, 4);
		arc.write(reinterpret_cast<char*>(&(m_Header.dirNameLength)), 1);
		arc.write(m_Header.dirName.c_str(), m_Header.dirNameLength);

		// then write file info for each file, along with the actual data
		for (std::list<file_info*>::iterator i = m_Files.begin(); i != m_Files.end(); i++){
			arc.write(reinterpret_cast<char*>(&((*i)->fileSize)), 4);
			arc.write(reinterpret_cast<char*>(&((*i)->compressedSize)), 4);
			arc.write(reinterpret_cast<char*>(&((*i)->fileNameLength)), 1);

			arc.write((*i)->fileName.c_str(), (*i)->fileNameLength);
			arc.write((*i)->data.m_Data, (*i)->compressedSize);
		}
	}
	std::cout << "Compressed directory created." << std::endl << std::endl;
}

void RLE_v2::ExtractArchive(const std::string& source)
{
	std::ifstream::pos_type size;
	char* memblock;

	std::ifstream file(source, std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[static_cast<unsigned int>(size)];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();
	}
	else {
		std::cout << "File not found." << std::endl << std::endl;
		return;
	}
	
	const char* temp = &(memblock[0]); // Start pointing at first element in the whole rl2 file

	// First four are header signature
	m_Header.sig[0] = *temp;
	m_Header.sig[1] = *(++temp);
	m_Header.sig[2] = *(++temp);
	m_Header.sig[3] = *(++temp);

	int count = 5;

	// Check that it's an RLE2 file
	if (m_Header.sig[0] != 'R' || m_Header.sig[1] != 'L' || m_Header.sig[2] != 'E' || m_Header.sig[3] != 0x02){
		std::cout << "Not a valid file to extract (must be RLE2)." << std::endl << std::endl;
		return;
	}
	else {
		m_Header.dirNameLength = *(reinterpret_cast<int*>(&memblock[4])); // Next element is directory name length

		temp = &memblock[5];
		for (int i = 0; i < m_Header.dirNameLength; i++){ // Grab directory name based on length
			m_Header.dirName += *(temp++);
			count++;
		}
	}

	while (count < size){ // While there are still elements in the RLE2 file...
		file_info *fileInfo = new file_info();

		// Store file information
		fileInfo->fileSize = *(reinterpret_cast<int*>(&memblock[count]));
		count += 4; // filesize and compressed size are 4 bits, because they are ints
		fileInfo->compressedSize = *(reinterpret_cast<int*>(&memblock[count]));
		count += 4;

		temp = &memblock[count];

		fileInfo->fileNameLength = *(temp);
		count++;

		temp = &memblock[count];

		for (int i = 0; i < fileInfo->fileNameLength; i++){ // Grab filename based on filenamelength
			fileInfo->fileName += *(temp);
			temp++;
			count++;
		}

		int sizeOfData = fileInfo->compressedSize;
		char * fileData = new char[sizeOfData];

		temp = &memblock[count];
	
		for (int i = 0; i < sizeOfData; i++){
			fileData[i] = *(&memblock[count]); // Make buffer of compressed data
			temp++;
			count++;
		}
	
		fileInfo->data.Decompress(fileData, sizeOfData, fileInfo->fileSize); // Decompress, store in fileInfo->data.m_Data

		m_Files.push_back(fileInfo);
	}

	CreateDirectory(m_Header.dirName.c_str(), NULL); // Create directory

	for (std::list<file_info*>::iterator i = m_Files.begin(); i != m_Files.end(); i++){
		std::string outputName = m_Header.dirName + "/" + (*i)->fileName; // name of each file, starting with directory + / so it goes in the proper folder
		std::ofstream arc(outputName, std::ios::out | std::ios::binary | std::ios::trunc);
		if (arc.is_open())
		{
			arc.write((*i)->data.m_Data, (*i)->fileSize); // Write the decompressed data for each file
		}
	}

	std::cout << "Files extracted successfully" << std::endl << std::endl;

	// Make sure to clean up!
	delete[] memblock;
}

RLE_v2::~RLE_v2()
{
	// Deconstructor to delete the file_info's in m_Files
	for (file_info *i : m_Files){
		delete i;
	}
}
