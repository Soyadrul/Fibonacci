#ifndef BIGINT_H
#define BIGINT_H

#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <algorithm>


class BigInt{
private:
    static const uint64_t EXPONENT = 18;
    static const uint64_t EXPONENTIATION_BASE = 10;
    static const uint64_t BASE = pow(EXPONENTIATION_BASE, EXPONENT);
    std::vector<uint64_t> vector_number;
    

public:

    static const int DIGIT_THRESHOLD_STANDARD = 2'048;       // When to switch algorithms
    static const int DIGIT_THRESHOLD_KARATSUBA = 200'000;

    BigInt(const std::string& _str_number);
    BigInt(const std::vector<uint64_t>& _vector_number);

    void set_string_number(const std::string& _str_number);

    std::string to_string() const;

    std::vector<uint64_t> get_vector_number() const;

    int get_num_digits() const;

    uint64_t get_base() const;

    uint64_t get_exponent() const;

    friend BigInt operator+(const BigInt& n1, const BigInt& n2);

    friend BigInt operator*(const BigInt& n1, const BigInt& n2);

    friend std::ostream& operator<<(std::ostream& os, const BigInt& number);
};

std::string vector_to_string(const std::vector<uint64_t>& vec, const uint64_t& digits_per_block);

#endif