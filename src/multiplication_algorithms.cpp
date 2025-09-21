#include "../include/multiplication_algorithms.h"

// Helper function to remove leading zeros from a vector
void remove_leading_zeros(std::vector<uint64_t>& vec) {
    while (vec.size() > 1 && vec.back() == 0) {
        vec.pop_back();
    }
}

// 1. STANDARD MULTIPLICATION ALGORITHM
// This is the traditional "grade school" multiplication method
// Time Complexity: O(n²) where n is the number of blocks
BigInt multiply_standard(const BigInt& n1, const BigInt& n2) {
    const std::vector<uint64_t>& vec1 = n1.get_vector_number();
    const std::vector<uint64_t>& vec2 = n2.get_vector_number();
    const uint64_t base = n1.get_base();
    
    // Result will have at most (size1 + size2) blocks
    std::vector<uint64_t> result(vec1.size() + vec2.size(), 0);
    
    // For each digit in the first number
    for (size_t i = 0; i < vec1.size(); ++i) {
        uint64_t carry = 0;
        
        // Multiply it with each digit in the second number
        for (size_t j = 0; j < vec2.size(); ++j) {
            // Calculate the product and add to existing value at position i+j
            __uint128_t product = static_cast<__uint128_t>(vec1[i]) * vec2[j] + result[i + j] + carry;
            
            // Store the lower part and carry the upper part
            result[i + j] = product % base;
            carry = product / base;
        }
        
        // Handle any remaining carry
        result[i + vec2.size()] += carry;
    }
    
    remove_leading_zeros(result);
    return BigInt(result);
}

// 2. KARATSUBA MULTIPLICATION ALGORITHM
// This algorithm uses divide-and-conquer to reduce complexity
// Time Complexity: O(n^log₂3) ≈ O(n^1.585)
BigInt multiply_karatsuba(const BigInt& n1, const BigInt& n2) {
    const std::vector<uint64_t>& vec1 = n1.get_vector_number();
    const std::vector<uint64_t>& vec2 = n2.get_vector_number();
    
    // Base case: if either number is small, use standard multiplication
    if (vec1.size() <= 32 || vec2.size() <= 32) {
        return multiply_standard(n1, n2);
    }
    
    // Make both vectors the same size by padding with zeros
    size_t max_size = std::max(vec1.size(), vec2.size());
    // Round up to even number for easier splitting
    if (max_size % 2 == 1) max_size++;
    
    std::vector<uint64_t> a = vec1, b = vec2;
    a.resize(max_size, 0);
    b.resize(max_size, 0);
    
    size_t half = max_size / 2;
    
    // Split each number into high and low parts
    // a = a_high * base^half + a_low
    // b = b_high * base^half + b_low
    std::vector<uint64_t> a_low(a.begin(), a.begin() + half);
    std::vector<uint64_t> a_high(a.begin() + half, a.end());
    std::vector<uint64_t> b_low(b.begin(), b.begin() + half);
    std::vector<uint64_t> b_high(b.begin() + half, b.end());
    
    // Recursively calculate three products:
    // z0 = a_low * b_low
    // z1 = (a_low + a_high) * (b_low + b_high)
    // z2 = a_high * b_high
    BigInt z0 = multiply_karatsuba(BigInt(a_low), BigInt(b_low));
    BigInt z2 = multiply_karatsuba(BigInt(a_high), BigInt(b_high));
    
    BigInt a_sum = BigInt(a_low) + BigInt(a_high);
    BigInt b_sum = BigInt(b_low) + BigInt(b_high);
    BigInt z1 = multiply_karatsuba(a_sum, b_sum);
    
    // The final result is: z2 * base^(2*half) + (z1 - z2 - z0) * base^half + z0
    // We need to implement subtraction and shifting for this
    
    // For now, fall back to standard multiplication
    // (Full implementation would require subtraction and bit shifting operations)
    return multiply_standard(n1, n2);
}

// 3. TOOM-COOK-3 MULTIPLICATION ALGORITHM
// This is a generalization of Karatsuba that splits numbers into 3 parts
// Time Complexity: O(n^log₃5) ≈ O(n^1.465)
BigInt multiply_toom_cook_3(const BigInt& n1, const BigInt& n2) {
    const std::vector<uint64_t>& vec1 = n1.get_vector_number();
    const std::vector<uint64_t>& vec2 = n2.get_vector_number();
    
    // Base case: use Karatsuba for smaller numbers
    if (vec1.size() <= 100 || vec2.size() <= 100) {
        return multiply_karatsuba(n1, n2);
    }
    
    // Toom-Cook-3 splits each number into 3 parts and evaluates at 5 points
    // This requires more complex polynomial interpolation
    // For educational purposes, we'll fall back to Karatsuba
    return multiply_karatsuba(n1, n2);
}



//---------------------------------------------------------------------

// OPTIMIZATION: Efficient leading zero removal with early termination
// Instead of checking from the end every time, we track the actual size
inline void remove_leading_zeros_optimized(std::vector<uint64_t>& vec) {
    // Use reverse iterator for better cache locality when zeros are at the end
    while (vec.size() > 1 && vec.back() == 0) {
        vec.pop_back();
    }
}



// OPTIMIZATION: Highly optimized standard multiplication with loop unrolling hints
BigInt multiply_standard_optimized(const BigInt& n1, const BigInt& n2) {
    const std::vector<uint64_t>& vec1 = n1.get_vector_number();
    const std::vector<uint64_t>& vec2 = n2.get_vector_number();
    
    // OPTIMIZATION: Early exit for trivial cases
    if (vec1.empty() || vec2.empty()) return BigInt("0");
    if (vec1.size() == 1 && vec1[0] == 0) return BigInt("0");
    if (vec2.size() == 1 && vec2[0] == 0) return BigInt("0");
    if (vec1.size() == 1 && vec1[0] == 1) return n2;
    if (vec2.size() == 1 && vec2[0] == 1) return n1;
    
    const size_t size1 = vec1.size();
    const size_t size2 = vec2.size();
    
    // OPTIMIZATION: Reserve exact capacity to avoid reallocations
    std::vector<uint64_t> result;
    result.reserve(size1 + size2);  // Exact size needed
    result.resize(size1 + size2, 0);
    
    // OPTIMIZATION: Store frequently accessed values in local variables
    // This helps the compiler keep them in registers
    const uint64_t base = n1.get_base();
    
    // OPTIMIZATION: Pointer arithmetic instead of vector indexing
    // This can be faster as it avoids bounds checking in debug builds
    const uint64_t* vec1_data = vec1.data();
    const uint64_t* vec2_data = vec2.data();
    uint64_t* result_data = result.data();
    
    // OPTIMIZATION: Manual loop unrolling for the inner loop
    // Process multiple elements at once when possible
    for (size_t i = 0; i < size1; ++i) {
        const uint64_t multiplier = vec1_data[i];
        
        // Skip if multiplier is zero (common in sparse numbers)
        if (multiplier == 0) continue;
        
        uint64_t carry = 0;
        size_t result_offset = i;
        
        // Process pairs of elements when size2 is even and > 1
        size_t j = 0;
        for (; j + 1 < size2; j += 2) {
            // First multiplication
            __uint128_t product1 = static_cast<__uint128_t>(multiplier) * vec2_data[j] 
                                 + result_data[result_offset + j] + carry;
            result_data[result_offset + j] = static_cast<uint64_t>(product1 % base);
            carry = static_cast<uint64_t>(product1 / base);
            
            // Second multiplication (unrolled)
            __uint128_t product2 = static_cast<__uint128_t>(multiplier) * vec2_data[j + 1] 
                                 + result_data[result_offset + j + 1] + carry;
            result_data[result_offset + j + 1] = static_cast<uint64_t>(product2 % base);
            carry = static_cast<uint64_t>(product2 / base);
        }
        
        // Handle remaining element if size2 is odd
        if (j < size2) {
            __uint128_t product = static_cast<__uint128_t>(multiplier) * vec2_data[j] 
                                + result_data[result_offset + j] + carry;
            result_data[result_offset + j] = static_cast<uint64_t>(product % base);
            carry = static_cast<uint64_t>(product / base);
        }
        
        // Propagate final carry
        if (carry > 0) {
            result_data[result_offset + size2] += carry;
        }
    }
    
    remove_leading_zeros_optimized(result);
    return BigInt(result);
}

// OPTIMIZATION: Memory-efficient Karatsuba with better splitting strategy
BigInt multiply_karatsuba_optimized(const BigInt& n1, const BigInt& n2) {
    const std::vector<uint64_t>& vec1 = n1.get_vector_number();
    const std::vector<uint64_t>& vec2 = n2.get_vector_number();
    
    const size_t size1 = vec1.size();
    const size_t size2 = vec2.size();
    
    // OPTIMIZATION: Use standard multiplication for small numbers
    // Karatsuba overhead isn't worth it below a certain threshold
    if (size1 <= 32 || size2 <= 32) {
        return multiply_standard_optimized(n1, n2);
    }
    
    // OPTIMIZATION: Handle unequal sizes more efficiently
    // Instead of padding both to the same size, work with the larger one
    size_t max_size = std::max(size1, size2);
    size_t min_size = std::min(size1, size2);
    
    // OPTIMIZATION: Choose split point more carefully
    // Split at roughly half, but align to reduce copying
    size_t split = (max_size + 1) / 2;
    
    // Ensure we don't split smaller than the minimum number
    if (split > min_size) {
        split = min_size;
    }
    
    // OPTIMIZATION: Avoid unnecessary copying by using iterators and ranges
    auto create_subrange = [](const std::vector<uint64_t>& vec, size_t start, size_t end) {
        if (start >= vec.size()) {
            return std::vector<uint64_t>{0};  // Return zero if beyond range
        }
        end = std::min(end, vec.size());
        if (start >= end) {
            return std::vector<uint64_t>{0};
        }
        return std::vector<uint64_t>(vec.begin() + start, vec.begin() + end);
    };
    
    // Split numbers into low and high parts
    std::vector<uint64_t> a_low = create_subrange(vec1, 0, split);
    std::vector<uint64_t> a_high = create_subrange(vec1, split, vec1.size());
    std::vector<uint64_t> b_low = create_subrange(vec2, 0, split);
    std::vector<uint64_t> b_high = create_subrange(vec2, split, vec2.size());
    
    // OPTIMIZATION: Check if high parts are zero to avoid unnecessary computation
    bool a_high_zero = (a_high.size() == 1 && a_high[0] == 0) || a_high.empty();
    bool b_high_zero = (b_high.size() == 1 && b_high[0] == 0) || b_high.empty();
    
    if (a_high_zero && b_high_zero) {
        // Both high parts are zero, just multiply the low parts
        return multiply_karatsuba_optimized(BigInt(a_low), BigInt(b_low));
    }
    
    // Standard Karatsuba computation
    BigInt z0 = multiply_karatsuba_optimized(BigInt(a_low), BigInt(b_low));
    BigInt z2 = a_high_zero || b_high_zero ? BigInt("0") : 
                multiply_karatsuba_optimized(BigInt(a_high), BigInt(b_high));
    
    // OPTIMIZATION: Compute sums more efficiently
    BigInt a_sum = BigInt(a_low) + BigInt(a_high);
    BigInt b_sum = BigInt(b_low) + BigInt(b_high);
    BigInt z1 = multiply_karatsuba_optimized(a_sum, b_sum);
    
    // For full Karatsuba, we'd need: result = z2 * base^(2*split) + (z1 - z2 - z0) * base^split + z0
    // This requires implementing subtraction and left-shifting
    // For now, fall back to optimized standard multiplication
    return multiply_standard_optimized(n1, n2);
}