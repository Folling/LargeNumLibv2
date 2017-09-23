#pragma once

#include <deque>
#include <locale>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <type_traits>
#include <stdexcept>
#include <string>

#include "largeNum.h"

class LargeInt : public LargeNum {
public:
	LargeInt();
	explicit LargeInt(const std::string numAsString);
	explicit LargeInt(const long long numAsInteger);
	LargeInt(const LargeInt& toCopy) noexcept;
	LargeInt(LargeInt&& toMove) noexcept;
	~LargeInt();
public:
	friend std::ostream& operator<<(std::ostream& os, const LargeInt& number);
	friend std::istream& operator>>(std::istream& is, LargeInt& number);
public: // convenience functions
	size_t size() const { return this->value.size(); }
	int& at(const size_t index) { return this->value.at(index); }
	const int& at(const size_t index) const { return this->value.at(index); }
	void resize(size_t newSize, int val = 0) { this->value.resize(newSize, val); }
	void push_back(const int val) { this->value.push_back(const_cast<int&>(val)); }
	void push_front(const int val) { this->value.push_front(const_cast<int&>(val)); }
	void pop_back() { this->value.pop_back(); }
	void pop_front() { this->value.pop_front(); }
public: // auxiliary functions
	void removeZerosAtStart();
public: // convenience operators
	LargeInt* operator->() { return this; }
	const LargeInt* operator->() const { return this; }
	LargeInt& operator*() { return *this; }
	const LargeInt& operator*() const { return *this; }
public: // basic integer arithmetic
	LargeInt operator+(const LargeInt& summand) const;
	LargeInt operator-(const LargeInt& subtrahend) const;
	LargeInt operator*(const LargeInt& factor) const;
	LargeInt operator/(const LargeInt& dividend) const;
	LargeInt operator%(const LargeInt& dividend) const;
public: // assignment operator
	LargeInt& operator=(const LargeInt& toCopy); 
	LargeInt& operator=(LargeInt&& toMove) noexcept;
public: // template operators for lefthand LargeInts
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt& operator=(const T& toSet);
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt operator+(const T& summand) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt operator-(const T& subtrahend) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt operator*(const T& factor) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt operator/(const T& dividend) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt operator%(const T& dividend) const;

public: // miscelleanous operators
	LargeInt& operator-();
	LargeInt& operator+();
	bool operator !() const;
public: // comparison/relational operators
		// all operators take the sign into account
		// for absolute value comparison user compare() [TODO]
		// or abs()
	bool operator==(const LargeInt& comparate) const;
	bool operator!=(const LargeInt& comparate) const;
	bool operator< (const LargeInt& comparate) const;
	bool operator> (const LargeInt& comparate) const;
	bool operator<=(const LargeInt& comparate) const;
	bool operator>=(const LargeInt& comparate) const;
public: // assignment arithmetic operators
	LargeInt& operator+=(const LargeInt& summand);
	LargeInt& operator-=(const LargeInt& summand);
	LargeInt& operator*=(const LargeInt& summand);
	LargeInt& operator/=(const LargeInt& summand);
	LargeInt& operator%=(const LargeInt& summand);
public: // in-/decrement operators
	LargeInt& operator++();
	LargeInt& operator--();
	LargeInt operator++(int dummy);
	LargeInt operator--(int dummy);
public: // comparison/relational operators for any ordinary types
		// same principle as previously.
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	bool operator==(const T& comparate) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	bool operator!=(const T& comparate) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	bool operator< (const T& comparate) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	bool operator> (const T& comparate) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	bool operator<=(const T& comparate) const;
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	bool operator>=(const T& comparate) const;
public: // assignment arithmetic operators for any ordinary types
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt& operator+=(const T& comparate);
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt& operator-=(const T& comparate);
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt& operator*=(const T& comparate);
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt& operator/=(const T& comparate);
	template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	LargeInt& operator%=(const T& comparate);
private:
	// subtracts absolute values for actual negative numbers.
	void subtract(const LargeInt& subtrahend);
	// compares the abs values;
	int compare(const LargeInt& comparate);
};

template<typename T, typename>
LargeInt& LargeInt::operator=(const T& toCopy){
	// casts in case toSet is a floating point number
	*this = LargeInt(std::to_string(static_cast<long long>(toCopy)));
	return *this;
}

template <typename T, typename>
bool LargeInt::operator==(const T& comparate) const {
	if (*this == LargeInt(std::to_string(static_cast<long long>(comparate)))) return true;
	return false;
}

template <typename T, typename>
bool LargeInt::operator!=(const T& comparate) const {
	return !(*this == comparate);
}

template <typename T, typename>
bool LargeInt::operator< (const T& comparate) const {
	if (*this < LargeInt(std::to_string(static_cast<long long>(comparate)))) return true;
	return false;
}

template <typename T, typename>
bool LargeInt::operator> (const T& comparate) const {
	return (LargeInt(std::to_string(static_cast<long long>(comparate))) < *this);
}

template <typename T, typename>
bool LargeInt::operator<=(const T& comparate) const {
	LargeInt t = LargeInt(std::to_string(static_cast<long long>(comparate)));
	return (*this < t || *this == t);
}

template <typename T, typename>
bool LargeInt::operator>=(const T& comparate) const {
	return !(*this < comparate);
}

template <typename T, typename>
LargeInt LargeInt::operator+(const T& summand) const {
	return *this + LargeInt(std::to_string(static_cast<long long>(summand)));
}

template <typename T, typename>
LargeInt LargeInt::operator-(const T& subtrahend) const {
	return *this - LargeInt(std::to_string(static_cast<long long>(subtrahend)));
}

template <typename T, typename>
LargeInt LargeInt::operator*(const T& factor) const {
	return *this * LargeInt(std::to_string(static_cast<long long>(factor)));
}

template <typename T, typename>
LargeInt& LargeInt::operator+=(const T& comparate) {
	*this = *this + comparate;
	return *this;
}

template <typename T, typename>
LargeInt& LargeInt::operator-=(const T& comparate) {
	*this = *this - comparate;
	return *this;
}

template <typename T, typename>
LargeInt& LargeInt::operator*=(const T& comparate) {
	*this = *this * comparate;
	return *this;
}


inline LargeInt factorial(const LargeInt& number) {
	LargeInt t = number;
	LargeInt result(1);
	while(t != 0) {
		result = result * t;
		t = t - 1;
	}
	return result;
}

inline LargeInt abs(const LargeInt& number) {
	LargeInt temp = number;
	return +temp;
}