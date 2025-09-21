#include <iostream>
#include <chrono>
#include <cstdint>
#include <array>
#include "../include/BigInt.h"
#include "../include/multiplication_algorithms.h"

// Returns the n-th biggest Fibonacci number calculated in 1 second
int stupid_fib() {
    BigInt n1("0");
    BigInt n2("1");

    BigInt temp("0");

    auto start = std::chrono::steady_clock::now();
    double loop_duration = 1.0;
    int i = 0;

    while(std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - start).count() < loop_duration) {
        temp = n2;
        n2 = temp + n1;
        n1 = temp;
        ++i;
        //std::cout << i << "°:\t" << n1 << "\t" << n1.get_num_digits() << "\n";
    }

    //std::cout << "\n\ncifre: " << n1.get_num_digits() << "\n\n"; 

    return i;
}





std::vector<std::vector<BigInt>> square_matrix(const std::vector<std::vector<BigInt>>& matrix) {
    std::vector<BigInt> row_result (2, BigInt("0"));
    std::vector<std::vector<BigInt>> result (2, row_result);

    /*
    Remember that matrix.at(0).at(1) is EQUAL to matrix.at(1).at(0)
    since the matrix has the following structure:

    Fn+1 Fn
    Fn   Fn-1
    */

    BigInt partial_0_1_1_0 = matrix.at(0).at(1) * matrix.at(1).at(0);
    BigInt partial_0_0_0_1 = matrix.at(0).at(0) * matrix.at(0).at(1);
    BigInt partial_0_1_1_1 = matrix.at(0).at(1) * matrix.at(1).at(1);
    BigInt sum = partial_0_0_0_1 + partial_0_1_1_1;

    result.at(0).at(0) = ( matrix.at(0).at(0) * matrix.at(0).at(0) ) + ( partial_0_1_1_0 );

    result.at(0).at(1) = sum;

    result.at(1).at(0) = sum;

    result.at(1).at(1) = ( partial_0_1_1_0 ) + ( matrix.at(1).at(1) * matrix.at(1).at(1) );

    
    return result;
}

void matrix_mul_identity(std::vector<std::vector<BigInt>>& matrix) {

    matrix.at(1).at(1) = matrix.at(0).at(1);
    matrix.at(0).at(1) = matrix.at(0).at(0);
    matrix.at(1).at(0) = matrix.at(0).at(0);
    matrix.at(0).at(0) = matrix.at(0).at(0) + matrix.at(1).at(1);
}

int matrix_fib() {
    std::vector<std::vector<BigInt>> matrix = 
    {
        {BigInt("1"), BigInt("1")},
        {BigInt("1"), BigInt("0")}
    };

    auto start = std::chrono::steady_clock::now();
    double loop_duration = 0.0001;
    int i = 1;

    while(std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - start).count() < loop_duration) {
        matrix = square_matrix(matrix);
        i *= 2;

        /*if(i > 200'000) {
            break;
        }*/
        std::cout << i - 1 << "°:\t" << matrix.at(1).at(1) << "\n";
    }

    //std::cout << "\n\ncifre: " << matrix.at(1).at(1).get_num_digits() << "\n\n"; 

    return i - 1;
}









int main() {

    //std::cout << stupid_fib() << "\n";
    std::cout << matrix_fib();

    
    return 0;
}