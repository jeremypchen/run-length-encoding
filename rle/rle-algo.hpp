#pragma once
#include <ostream>
#include <vector>

// RLE Compression/Decompression
template <typename T>
struct RLE
{
	// Memory which stores either compressed or decompressed data
	T* m_Data;
	// Number of elements of type T that data is pointing to
	int m_Size;

	RLE()
	: m_Data(nullptr)
	, m_Size(0)
	{ }

	~RLE()
	{
		delete m_Data;
	}
	
	// Compresses input data and stores it in m_Data
	void Compress(const T* input, int inSize);

	// Decompresses input data and stores it in m_Data
	void Decompress(const T* input, int inSize, int outSize);

	// Outputs m_Data
	friend std::ostream& operator<< (std::ostream& stream, const RLE<T>& rhs)
	{
		for (int i = 0; i < rhs.m_Size; i++)
		{
			stream << rhs.m_Data[i];
		}
		return stream;
	}

	// Shenanigans to get the maximum run size
	int MaxRunSize();
};

template <typename T>
void RLE<T>::Compress(const T* input, int inSize)
{
	delete m_Data;
	m_Size = 0;

	m_Data = new T[inSize * 2];

	int maxRunSize = MaxRunSize(); // Get the max run size that can fit in one char
	
	const T* temp = &(input[0]); // point to the first element of the input 

	int runCount = 0;
	int count = 0;
	std::vector<char> negativeString;
	while (count < inSize) // Keep RLE algorithm running while there are elements in the input
	{
		if (*(temp + 1) == *temp ) // If elements are the same, then go on a positive run
		{
			T letter = *temp;
			runCount = 2; // 2 because it accounts for the first two elements that are the same that triggered the positive run
			temp++;
			while (*(temp + 1) == *temp && count < inSize && runCount < maxRunSize) // while consecutive elements still
			{
				runCount++;
				temp++;
			}
			m_Data[m_Size] = runCount; // Once run is over, put run count first
			m_Size++;
			m_Data[m_Size] = letter; // Then put in the letter that was consecutive
			m_Size++;
			count += runCount;
			runCount = 0;
			temp++;
		}
		else // If elements are not the same, go on a negative run
		{
			negativeString.push_back(*temp); // Keep track of the letters in a string
			runCount = 1;
			count++;
			temp++;
			while (*(temp + 1) != *temp && count < inSize && runCount < maxRunSize){ // While they still don't match...
				runCount++; // Keep track of the number of elements 
				count++;
				negativeString.push_back(*temp); // Keep appending the letters
				temp++;
			}
			m_Data[m_Size] = runCount * (-1); // Make this number negative
			m_Size++;
			for (size_t i = 0; i < negativeString.size(); i++) { // Insert the negative string
				m_Data[m_Size] = negativeString[i];
				m_Size++;
			}
			runCount = 0;
			negativeString.clear();
		}
	}
}

template <typename T>
void RLE<T>::Decompress(const T* input, int inSize, int outSize)
{
	delete m_Data;
	m_Size = 0;

	m_Data = new T[outSize];

	int runCount;
	int count = 0;

	const T* temp = &(input[0]); // Point to the first element

	while (count < inSize){ 
		runCount = *temp;
		temp++;
		count++;

		if (runCount < 0) // Negative run
		{
			runCount = (runCount * -1);
			while (runCount != 0){
				m_Data[m_Size] = *temp; // Store the letters
				m_Size++;
				temp++;
				runCount--; // Decrement the runcount until it's 0
				count++;
			}
		}
		else // Positive run 
		{
			while (runCount != 0){
				m_Data[m_Size] = *temp; // Store the letters
				m_Size++;
				runCount--; // Decrement the runcount until it's 0
			}
			temp++;
			count++;
		}
	}
}
