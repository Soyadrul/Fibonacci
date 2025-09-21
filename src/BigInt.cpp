#include "../include/BigInt.h"
#include "../include/multiplication_algorithms.h"

BigInt::BigInt(const std::string& _str_number) {
    this->set_string_number(_str_number);
}

BigInt::BigInt(const std::vector<uint64_t>& _vector_number) {
    vector_number = _vector_number;
}

void BigInt::set_string_number(const std::string& _str_number) {
    vector_number.clear();
    vector_number.resize(
        ceil( static_cast<double>( _str_number.size() ) / static_cast<double>(EXPONENT) ), 
        0
    );

    int vector_index = 0;
    int position = 0;
    for(int i = _str_number.size() - 1; i >= 0 ; --i) {
        if(position >= EXPONENT) {
            position = 0;
            ++vector_index;
        }

        vector_number.at(vector_index) += ( _str_number.at(i) - '0' ) * ( pow(EXPONENTIATION_BASE, position) );
        ++position;
    }
}

std::string vector_to_string(const std::vector<uint64_t>& vec, const uint64_t& digits_per_block) {
    std::string str = "";
    
    for(int i = 0; i < vec.size() - 1; ++i) {
        std::string s = std::to_string(vec.at(i));

        int num_of_zeros = digits_per_block - s.size();
        while(num_of_zeros > 0) {
            s.insert(0, "0");
            --num_of_zeros;
        }

        str.insert(0, s);
    }
    str.insert(0, std::to_string(vec.at( vec.size() - 1 )));

    return str;
}

std::string BigInt::to_string() const {
    return vector_to_string(this->get_vector_number(), this->get_exponent());
}

std::vector<uint64_t> BigInt::get_vector_number() const {
    return vector_number;
}

int BigInt::get_num_digits() const {
    return this->to_string().size();
}

uint64_t BigInt::get_base() const {
    return this->BASE;
}

uint64_t BigInt::get_exponent() const {
    return this->EXPONENT;
}

BigInt operator+(const BigInt& n1, const BigInt& n2) {
    uint64_t carry = 0;
    const uint64_t n1_base = n1.get_base();
    std::vector<uint64_t> n1_vector = n1.get_vector_number();
    std::vector<uint64_t> n2_vector = n2.get_vector_number();
    int n1_num_digits = n1_vector.size();
    int n2_num_digits = n2_vector.size();

    int min_size = ( n1_num_digits <= n2_num_digits) ? n1_num_digits : n2_num_digits ;
    int max_size = ( n1_num_digits >= n2_num_digits) ? n1_num_digits : n2_num_digits ;


    std::vector<uint64_t> sum(max_size + 1, 0);

    for(int i = 0; i < min_size; ++i) {
        sum.at(i) = n1_vector.at(i) + n2_vector.at(i) + carry;
        carry = sum.at(i) / n1_base;
        sum.at(i) %= n1_base;
    }

    for(int j = min_size ; j < max_size; ++j) {
        sum.at(j) += (n1_num_digits >= n2_num_digits) ? n1_vector.at(j) : n2_vector.at(j);
        sum.at(j) += carry;
        carry = sum.at(j) / n1_base;
        sum.at(j) %= n1_base;
    }
    sum.at(max_size) += carry;

    if( sum.at( sum.size() - 1) == 0 ) {
        sum.erase(sum.begin() + (sum.size() - 1));
    }

    return BigInt(sum);
}

std::ostream& operator<<(std::ostream& os, const BigInt& number) {
    os << number.to_string();
    return os;
}

// OPTIMIZATION: Smart algorithm selection with hysteresis
// This prevents thrashing between algorithms for numbers near thresholds
BigInt operator*(const BigInt& n1, const BigInt& n2) {
    const size_t size1 = n1.get_vector_number().size();
    const size_t size2 = n2.get_vector_number().size();
    const size_t total_size = size1 + size2;
    const size_t max_size = std::max(size1, size2);
    
    // OPTIMIZATION: Consider both total work and maximum dimension
    // This gives better algorithm selection for unbalanced multiplications
    if (max_size <= n1.DIGIT_THRESHOLD_STANDARD || 
        total_size <= n1.DIGIT_THRESHOLD_STANDARD) {
        return multiply_standard_optimized(n1, n2);
    } else if (max_size <= n1.DIGIT_THRESHOLD_KARATSUBA || 
               total_size <= n1.DIGIT_THRESHOLD_KARATSUBA) {
        return multiply_karatsuba_optimized(n1, n2);
    } else {
        return multiply_toom_cook_3(n1, n2);
    }
}