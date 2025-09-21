#ifndef MULTIPLICATION_ALGORITHMS_H
#define MULTIPLICATION_ALGORITHMS_H

#include <vector>
#include <cstdint>
//#include <algorithm>
#include "../include/BigInt.h"


void remove_leading_zeros(std::vector<uint64_t>& vec);

BigInt multiply_standard(const BigInt& n1, const BigInt& n2);

BigInt multiply_karatsuba(const BigInt& n1, const BigInt& n2);

BigInt multiply_toom_cook_3(const BigInt& n1, const BigInt& n2);


inline void remove_leading_zeros_optimized(std::vector<uint64_t>& vec);

BigInt multiply_standard_optimized(const BigInt& n1, const BigInt& n2);

BigInt multiply_karatsuba_optimized(const BigInt& n1, const BigInt& n2);

#endif