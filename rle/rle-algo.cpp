#include "rle-algo.hpp"
#include <type_traits>

template <typename T> // by default can accept any type
int RLE<T>::MaxRunSize()
{
	static_assert(false, "RLE not supported for this type."); 
	//std::is_integral<T>::value, "RLE not supported for this type."); // (false, .. ) way to tell compiler, give a compiler error
	
	//return (1 << ((sizeof(T)* 8 - 1)) -1;
}

// different specializations
template <>
int RLE<char>::MaxRunSize()
{
	return 127;
}

template <>
int RLE<short>::MaxRunSize()
{
	return 32767;
}

template <>
int RLE<int>::MaxRunSize()
{
	return 2147483647;
}
