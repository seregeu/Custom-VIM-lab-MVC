#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <initializer_list> 
class MyString
{
private:
	char* string_;
	size_t string_len_;
	size_t capacity_;
	const size_t gCapacityFactor = 15;
	size_t CapacityCounter(const size_t string_len)const;
	const int cTempBufSize = 10000;
public:
	// default constructor
	MyString();
	//copy constructor
	MyString(const MyString& input_string);
	// char array constructor
	MyString(const char* input_string);
	// initializer list constructor 
	MyString(const std::initializer_list<char>& list);
	// std::string constructor
	MyString(const std::string& cString);
	// init class with count characters of  “char string”,Constructor
	MyString(const char* input_string, const int count);
	//// init class with count of characters Constructor
	MyString(int count, char letter);
	~MyString();
	//output operator
	friend std::ostream& operator<<(std::ostream& out, const MyString& cString);
	//input operator
	friend std::istream& operator>> (std::istream& in, MyString& string_);
	// concatenate with Mystring
	MyString operator+(const MyString& first_string);
	void operator=(const MyString& source);
	// concatenate with char array
	MyString operator+(const char* first_string);
	friend MyString operator+(const char* first_string, MyString& second_string);
	// concatenate with std::string
	MyString operator+(const std::string& first_string);
	friend MyString operator+(const char* first_string, MyString& second_string);
	friend MyString operator+(const std::string& first_string, MyString& second_string);
	// assignment concatenate with char array
	void operator+=(const char* add_string);
	// assignment concatenate with std::string
	void operator+=(const std::string& add_string);
	//assignment concatenate with MySrting
	void operator+=(const MyString& add_string);

	// char string assignment
	void operator=(const char* source);
	// std::string assignment
	void operator=(const std::string& source);
	// char assignment
	void operator=(const char source);
	// index operator
	char operator[](const size_t element)const;

	// lexicographically comparing >
	friend bool operator>(const MyString& firts_string, const MyString& second_string);//
	friend bool operator>(const MyString& firts_string, const char* second_string);
	friend bool operator>(const char* firts_string, const MyString& second_string);
	friend bool operator>(const MyString& firts_string, const std::string& second_string);
	friend bool operator>(const std::string& firts_string, const MyString& second_string);

	// lexicographically comparing <
	friend bool operator<(const MyString& firts_string, const MyString& second_string);//
	friend bool operator<(const MyString& firts_string, const char* second_string);
	friend bool operator<(const char* firts_string, const MyString& second_string);
	friend bool operator<(const MyString& firts_string, const std::string& second_string);
	friend bool operator<(const std::string& firts_string, const MyString& second_string);

	// lexicographically comparing >=
	friend bool operator>=(const MyString& firts_string, const MyString& second_string);//
	friend bool operator>=(const MyString& firts_string, const  char* second_string);
	friend bool operator>=(const char* firts_string, const  MyString& second_string);
	friend bool operator>=(const MyString& firts_string, const std::string& second_string);
	friend bool operator>=(const std::string& firts_string, const  MyString& second_string);

	// lexicographically comparing <=
	friend bool operator<=(const MyString& firts_string, const  MyString& second_string);//
	friend bool operator<=(const MyString& firts_string, const  char* second_string);
	friend bool operator<=(const char* firts_string, const MyString& second_string);
	friend bool operator<=(const MyString& firts_string, const std::string& second_string);
	friend bool operator<=(const std::string& firts_string, const  MyString& second_string);

	// lexicographically comparing ==
	friend bool operator==(const MyString& firts_string, const  MyString& second_string);//
	friend bool operator==(const MyString& firts_string, const char* second_string);
	friend bool operator==(const char* firts_string, const  MyString& second_string);
	friend bool operator==(const MyString& firts_string, const std::string& second_string);
	friend bool operator==(const std::string& firts_string, const  MyString& second_string);

	// lexicographically comparing !=
	friend bool operator!=(const MyString& firts_string, const MyString& second_string);//
	friend bool operator!=(const MyString& firts_string, const char* second_string);
	friend bool operator!=(const char* firts_string, const  MyString& second_string);
	friend bool operator!=(const MyString& firts_string, const std::string& second_string);
	friend bool operator!=(const std::string& firts_string, const MyString& second_string);

	// return a pointer to null-terminated character array
	const char* c_str()const;
	// return a pointer to array data that not required to be null-terminated
	const char* data()const;
	// same as size 
	size_t length()const;
	// return the number of char elements in string
	size_t size()const;
	// true if string is empty
	bool empty()const;
	// return the current amount of allocated memory for array
	size_t capacity()const;
	// reduce the capacity to size
	void shrink_to_fit();
	// remove all char element in string
	void clear();

	// insert count of char in index position
	void insert(const size_t index, const size_t count, const char elem);
	// insert count of null-terminated char string at index position
	void insert(const size_t index, const char* string, const size_t count);
	// insert null-terminated char string at index position
	void insert(const size_t index, const char* string);
	// insert count of std::string at index position
	void insert(const size_t index, const std::string& string, const size_t count);
	// insert std::string at index position
	void insert(const size_t index, const std::string& string);

	// erase count of char at index position
	void erase(const size_t index, const size_t count);

	// append count of char
	void append(const size_t count, const char letter);
	// append null-terminated char string
	void append(const char* string);
	// append a count of null-terminated char string by index position
	void append(const char* string, const size_t index, const size_t count);
	// append std:: string
	void append(const std::string& string);
	// append a count of std:: string by index position
	void append(const std::string& string, const size_t index, const size_t count);
	//MyString append
	void append(const MyString& string);
	void append(const MyString& string, const size_t index, const size_t count);

	// replace a count of char at index by “string”
	void replace(const size_t index, const size_t count, const char* string);
	// replace a count of char at index by std::string
	void replace(const size_t index, const size_t count, const std::string& string);

	//return a substring starts with index position
	MyString substr(const size_t index)const;
	// return a count of substring’s char starts with index position
	MyString substr(const size_t index, const size_t count)const;

	// if founded return the index of substring
	size_t find(const char* string)const;
	// same as find(“string”) but search starts from index position
	size_t find(const char* string, const size_t index)const;
	// if founded return the index of substring
	size_t find(const std::string& string)const;
	// same as find(“string”) but search starts from index position
	size_t find(const std::string& string, const size_t index)const;

	void MemoryRelocation(const char* string);
};

class MyStringPython : public MyString
{
public:
	MyStringPython() {};
	MyStringPython(const MyStringPython& other) : MyString(other) {};

	MyStringPython(const char* const cchar_array) : MyString(cchar_array) { };
	MyStringPython(const std::string& std_string) : MyString(std_string) { };

	MyStringPython(const size_t count, const char c) : MyString(count, c) {};
	//MyStringPython(const std::initializer_list<char>& list) : MyString (list); //don't work in Python
	MyStringPython(const char* const cchar_array, const size_t count) : MyString(cchar_array, count) { };

	using MyString::operator=; //do not works in python
	using MyString::operator+;
	//using MyString::operator<;
	//using MyString::operator>;
	//using MyString::operator<=;
	//using MyString::operator>=;
	//using MyString::operator==;
	//using MyString::operator!=;
	using MyString::operator+=;
	using MyString::operator[]; //do not works in python

	~MyStringPython() {};

	//for Python
	const char* __str__();
	void __setitem__(size_t i, char c);
	const char __getitem__(size_t i);

	//= for Python
	void equall_c(char c);
	void equall_std_string(const std::string& std_string);
	void equall_char_array(const char* cchar_array);
};