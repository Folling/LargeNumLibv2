#include "LargeInt.h"
#include <chrono>

const LargeInt zero(0);
const LargeInt negZero(-0);
const LargeInt one(1);
const LargeInt ten(10);

LargeInt::LargeInt() {
	value = {0};
	sign = '+';
}

LargeInt::LargeInt(std::string numAsString) {
	if(numAsString[0] == '-') {
		this->sign = '-';
		// cuts sign out
		numAsString.at(0) = '0';
	} else this->sign = '+';
	// 0s at the start are being ignored
	// though 0s at the end are actually relevant.
	bool fDigit = false;
	for (size_t i = 0; i < numAsString.size(); i++) {
		// numbers and letters don't mix
		if(!std::isdigit(static_cast<unsigned char>(numAsString.at(i)))) {
			using namespace std::string_literals;
			value.clear();
			sign = '+';
			// might show error message in Visual studio, but trust me, it runs.
			std::string errorMsg = "Unable to parse character: "s += numAsString.at(i);
			throw std::runtime_error(errorMsg);
		}
		if(fDigit || numAsString.at(i) != '0') {
			value.push_back((numAsString.at(i) - '0'));
			fDigit = true;
		}		
	}
	if (value.size() == 0) value.push_back(0);
}

LargeInt::LargeInt(long long numAsInteger) {
	std::string numAsString = std::to_string(numAsInteger);
	*this = LargeInt(numAsString);
}

LargeInt::LargeInt(const LargeInt& toCopy) noexcept {
	if (this != &toCopy) {
		this->value = toCopy.value;
		this->sign = toCopy.sign;
	}
}

LargeInt::LargeInt(LargeInt&& toMove) noexcept {
	if (this != &toMove) {
		this->value = toMove.value;
		this->sign = toMove.sign;
		toMove.sign = '+';
	}
}


LargeInt::~LargeInt() {
}

std::ostream& operator<<(std::ostream& os, const LargeInt& number) {
	if(number.sign == '-') os << '-';
	for(auto i : number.value) {
		os << i;
	}
	return os;
}

std::istream& operator>>(std::istream& is, LargeInt& number) {
	std::string val;
	is >> val;
	try {
		number = LargeInt(val);
	} catch(const std::runtime_error& e) {
		throw e;
	} // TODO fix rethrow
	number.removeZerosAtStart();
	is.clear();
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return is;
}

void LargeInt::removeZerosAtStart() {
	if (this->size() == 0) return;
	while (this->at(0) == 0 && this->size() != 1) this->pop_front();
}

LargeInt LargeInt::operator+(const LargeInt& summand) const {
	// 0 + x == x
	if(this == 0) return summand;
	if(summand == 0) return *this;	

	size_t max = std::max(this->size(), summand.size());

	// we have to adapt the numbers to the same size
	// since we add them digit by digit
	bool sameSize = this->size() == summand.size();
	LargeInt larger = (sameSize? *this : this->size() > summand.size()? *this : summand);
	LargeInt smaller = (sameSize? summand : this->size() > summand.size()? summand : *this);
	LargeInt result;

	smaller.value.insert(smaller.value.begin(), max - smaller.size(), 0);
	int carryOver = 0;
	if(this->sign == summand.sign) {
		// we start at the back of the number as we have to take
		// carry-overs ( x+y >= 10) into account
		// basically what we do here is the addition you'd learn in 
		// primary school. we add up each number and if it's larger than 
		// ten we only take the second digit into account 
		// which is equivalent to x%10
		// if that was the case we carry over a one to the next addition
		for(int i = max - 1; i >= 0; i--) {
			auto val = carryOver + larger->at(i) + smaller.at(i);
			if(val >= 10) {
				result.push_front(val % 10);
				carryOver = 1;
			} else {
				result.push_front(val);
				carryOver = 0;
			}
		}
		// for some reason there's always a 0 at the end
		// I can't quite figure out why, but well it is.
		result.pop_back();
		if(carryOver) result.push_front(1);
		// adding two negative numbers together always ends up being
		// another negative number
		if(this->sign == '-') result.sign = '-';
	} else { // different signs
		if (this->value == summand.value) return result;
		// same method as previously, hopefully just more efficient
		// we subtract by adding the complement 
		// and crossing out the first digit

		// determines the negative and the positive number
		LargeInt negative = (smaller.sign == '-' ? smaller : larger);
		LargeInt nonNegative = (negative == smaller ?  larger : smaller);
		bool largerNegative = negative.value > nonNegative.value;

		// subtracts the given result because y'know
		// let's say x = negative 
		// and y = positive
		// if |x| > |y| the resulting number will be negative
		// it will be exactly the |x| - |y| negative:
		// 5 - 8 == -(8-5)
		// x - y == -(x-y) for x < 0 && y > 0 && |x| > |y|
		// otherwise it's basically just subtracting because x + (-y) == x - y
		if(largerNegative) {
			negative.subtract(nonNegative);
			result = negative;
			result.sign = '-';
		} else {
			nonNegative.subtract(negative);
			result = nonNegative;
			result.sign = '+';
		}
	}
	// every operator guarantees to return the result without leading 0s
	result.removeZerosAtStart();
	return result;
}

LargeInt LargeInt::operator-(const LargeInt& subtrahend) const {
	// x - y == x + (-y)
	return *this + (-const_cast<LargeInt&>(subtrahend));
}

LargeInt LargeInt::operator*(const LargeInt& factor) const {
	LargeInt result;

	if (*this  == 0 || factor == 0) return result;
	if (*this  == 1) return factor;
	if (factor == 1) return *this;

	LargeInt toAdd;

	int carryOver = 0;
	// iterates through each digit
	// the method used is the same you learned in primary school
	// we basically just create multiple numbers to add together.
	for (int i = factor.size() - 1; i >= 0; i--){
		for (int j = this->size() - 1; j >= 0; j--)	{
			int temp = this->at(j) * factor.at(i) + carryOver;
			carryOver = temp / 10;
			toAdd.push_front(temp % 10);
		}
		// tenfolds the current multiplication by the amount of previous multiplications
		int zeroExtender = factor.size() - 1 - i;
		toAdd.value.insert(toAdd.value.end(), zeroExtender, 0);

		// if we still had some digits over, this makes it the new last digit.
		if(carryOver != 0) toAdd.push_front(carryOver);

		// then it adds the previously calculated result to the current one and resets it
		result = result + toAdd;
		carryOver = 0;
		toAdd = 0;
	}
	// there's always a 0 too much at the end
	result.pop_back();
	// we just multiplied the values
	// this determines the sign of the result
	// pos * pos = pos;
	// neg * neg = pos;
	// neg * pos = neg;
	if (this->sign != factor.sign) result.sign = '-';
	result.removeZerosAtStart();
	return result;
}

LargeInt& LargeInt::operator=(const LargeInt& toCopy) {
	if (this != &toCopy) {
		this->sign = toCopy.sign;
		this->value = toCopy.value;
	}
	return *this;
}

LargeInt& LargeInt::operator=(LargeInt&& toMove) noexcept {
	if (this != &toMove) {
		this->sign = toMove.sign;
		this->value = std::move(toMove.value);
		toMove.sign = '+';
	}
	return *this;
}

LargeInt& LargeInt::operator-() {
	this->sign = (this->sign == '+' ? '-' : '+');
	return *this;
}

LargeInt& LargeInt::operator+() {
	this->sign = '+';
	return *this;
}

bool LargeInt::operator==(const LargeInt& comparate) const {
	if (this->sign == comparate.sign && this->value == comparate.value) return true;
	if (this->at(0) == 0 && comparate.at(0) == 0) return true;
	return false;
}

bool LargeInt::operator< (const LargeInt& comparate) const {
	if (this->at(0) == 0 && comparate.at(0) == 0) return false;
	if (this->sign == '-' && comparate.sign == '+') return true;
	if (this->sign == '+' && comparate.sign == '-') return false;
	// negative numbers are smaller if their absolute values are larger
	if (this->sign == '-') {
		if (this->size() > comparate.size()) return true;
		if (comparate.size() > this->size()) return false;
		if (this->value > comparate.value) return true;
		else return false;
	}
	if (this->size() < comparate.size()) return true;
	if (comparate.size() < this->size()) return false;
	if (this->value < comparate.value) return true;
	return false;
}

bool LargeInt::operator> (const LargeInt& comparate) const {
	return (comparate < *this);
}

bool LargeInt::operator!=(const LargeInt& comparate) const {
	return !(*this == comparate);
}

bool LargeInt::operator<=(const LargeInt& comparate) const {
	return !(comparate < *this);
}

bool LargeInt::operator>=(const LargeInt& comparate) const {
	return !(*this < comparate);
}

LargeInt& LargeInt::operator+=(const LargeInt& summand) {
	*this = *this + summand;
	return *this;
}

LargeInt& LargeInt::operator-=(const LargeInt& summand) {
	*this = *this - summand;
	return *this;
}

LargeInt& LargeInt::operator*=(const LargeInt& summand) {
	*this = *this * summand;
	return *this;
}

LargeInt& LargeInt::operator++() {
	*this += 1;
	return *this;
}

LargeInt& LargeInt::operator--() {
	*this -= 1;
	return *this;
}

LargeInt LargeInt::operator++(int dummy) {
	LargeInt temp = *this;
	*this += 1;
	return temp;
}

LargeInt LargeInt::operator--(int dummy) {
	LargeInt temp = *this;
	*this -= 1;
	return temp;
}

bool LargeInt::operator!() const{
	if (this->size() == 0 || (this->at(0) == 0 && this->size() == 1)) return true;
	return false;
} 

void LargeInt::subtract(const LargeInt& subtrahend) {
	assert(this->size() == subtrahend.size());
	int carryOver = 0;
	for (int i = this->size() - 1; i >= 0; i--) {
		if (this->at(i) - subtrahend.at(i) - carryOver < 0) {
			this->at(i) = this->at(i) + 10 - subtrahend.at(i) - carryOver;
			carryOver = 1;
		}
		else {
			this->at(i) = this->at(i) - subtrahend.at(i) - carryOver;
			carryOver = 0;
		}
	}
}

int LargeInt::compare(const LargeInt& comparate) {
	if (abs(*this) > abs(comparate)) return 1;
	if (abs(*this) < abs(comparate)) return -1;
	return 0;
}
