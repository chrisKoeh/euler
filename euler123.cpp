#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cassert>
#include <chrono>
#include <math.h>

#include "BigInt.h"

#define PRIME_MAX 3500

using namespace PositiveBigInt;
using namespace std::chrono;

constexpr unsigned long long B_EDGE = 1e12;
constexpr unsigned long long N_MAX = 2.7e6;
constexpr unsigned long long e12 = 2617253;
constexpr unsigned long long e11 = 790547;
constexpr unsigned long long e10 = 237737;
std::vector<unsigned long long> primes = {2,3,5,7};
std::array<unsigned long, N_MAX> solutions = {};
std::array<int, N_MAX> solution_indices = {};

bool is_prime(unsigned long long num) {
    if(num < 2) return false;
    for(const auto &val:primes) {
        if(val > num / val) return true;
        if(num % val == 0) return false;
    }
    return true;
}

void find_primes_to_n() {
    for(unsigned long long i = 9; i < N_MAX; i+=2) {
        if(is_prime(i)) primes.push_back(i);
    }
}

// a^b
unsigned long long power_of_n(const unsigned long long& a, const unsigned long long& b, const unsigned long long& divisor = 1e18)
{
    if( b == 0 ) return 1;

    constexpr int max_digits = 35;
    int c = 1;
    int total_c = 1;
    unsigned long long res = a;
    unsigned long long factor = a;
    std::map<int, unsigned long long> factor_map;
    while( total_c < b )
    {
        unsigned long long res_ = res * factor;

        // handle overflow
        if( res_ / factor != res )
        {
            BigInt b_res = BigInt(factor, 0, max_digits );
            b_res *= res;// BigInt(res, 0 , max_digits);
            res = b_res.modulo(divisor); //b_res.modulo(divisor);
        }
        else
        {
            res = res_ % divisor;
        }

        c *= 2;
        if( factor_map[c] == 0 )
        {
            unsigned long long factor_ = factor * factor;

            // handle overflow
            if( factor_ / factor != factor )
            {
                BigInt b_res = BigInt(factor, 0, max_digits );
                b_res *= b_res;
                factor = b_res.modulo(divisor);
            }
            else
            {
                factor = factor_ % divisor;
            }
            factor_map[c] = factor;
        }
        else
        {
            factor = factor_map[c];
        }
        total_c += c/2;
        while( c + total_c > b )
        {
            c /= 2;
            factor = factor_map[c];
            if( c == 1 )
            {
                factor = a;
                break;
            }
        }
    }
    return res;
}

unsigned long long calc_remainder( int n )
{
    const unsigned long long prime = primes[ n-1 ];
    const unsigned long long divisor = prime * prime;
    const unsigned long long prime_plus = prime + 1;
    const unsigned long long prime_minus = prime - 1;


    unsigned long long prime_plus_res = power_of_n( prime_plus, n, divisor );
    unsigned long long prime_minus_res = power_of_n( prime_minus, n, divisor );
    unsigned long long res = ((prime_plus_res % divisor) + (prime_minus_res % divisor )) % divisor;
    return res;
}

void unit_power_test(unsigned long long a, unsigned long long b, unsigned long long expected)
{
    std::cout << "TEST: " << a << "^" << b << "=";
    auto res = power_of_n(a, b);
    std::cout << res << " and expected: " << expected << std::endl;
    assert( res == expected );
}

void unit_power_tests()
{
    /*unit_power_test(2,1, 2);
    unit_power_test(2,3, 8);
    unit_power_test(2,4, 16);*/
    unit_power_test(3,9, 19683);
    unit_power_test(3,10, 59049);
    unit_power_test(3,11, 177147);
    unit_power_test(3,23, 94143178827);
}

unsigned long long calc_larger_e11(unsigned long long num, long c = 0, long a = 549893, long b = 19303243 )
{
    return (a*log2((num + c)) - b);
}

unsigned long long calc_larger_e10(unsigned long long num)
{
    long a = 166412;
    long b = 5290363;
    return (a*log2((num)) - b) - 1000;
}

int do_main()
{
    std::ios::sync_with_stdio(false);
    int T;
    //std::cin >> T;
    std::vector<unsigned long long> inputs;
    unsigned long long N;
    /*for(int i = 0; i < T; i++)
    {
        std::cin >> N;
        inputs[i] = N;
    }*/

    find_primes_to_n();
    solutions[3] = 2;
    solution_indices[3] = 2;
    for( int n = 1; n <= 200000; n+=2 )
    {
        unsigned long long res = calc_remainder(n);
        solutions[ primes[n-1] ] = res;
        solution_indices[ primes[n-1] ] = n;
        if( res >= B_EDGE)
        {
            break;
        }
    }

    unsigned long factor = 2;
    for(int i = 0; i < 39; i++)
    {
        inputs.push_back(factor);
        factor *= 2;
    }
    factor = 10;
    for(int i = 0; i < 12; i++)
    {
        inputs.push_back(factor);
        factor *= 10;
    }
    for(int i = 0; i < 100; i++)
    {
        inputs.push_back((unsigned long long)i*1e10);
    }
    factor = 10;
    for(int i = 0; i < 12; i++)
    {
        inputs.push_back(factor);
        factor *= 10;
    }
    inputs.push_back(1e12);
    for(int i = 0; i < 10000; i++)
    {
        inputs.push_back((unsigned long long)i*1e8);
    }

    std::vector<int> out_to_validate;
    for(const auto& input: inputs)
    {
        for(int i = 0; i < primes.size(); i++)
        {
            const auto prime = primes[i];
            if( solutions[prime] > input )
            {
                out_to_validate.push_back(i+1);
                break;
            }
        }
    }

    int out_p = 0;
    for(const auto& input: inputs)
    {
        int i_start = 0;
        int prime_1eX = 21089;
        const int thres_eX = 1e8;
        if( input > thres_eX )
        {
            unsigned long start_N = thres_eX;
            i_start = solution_indices[prime_1eX];
            while( start_N*22/10 < input)
            {
                prime_1eX = (long double)prime_1eX/log(2);
                start_N *= 2;
            }

            while( (start_N*130)/100 < input)
            {
                prime_1eX = (long double)prime_1eX/log(1.1);
                prime_1eX /= 10;
                start_N *= 11;
                start_N /= 10;
            }

            while( solutions[prime_1eX] == 0 ) prime_1eX--;
            i_start = solution_indices[prime_1eX];
        }
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = i_start; i < primes.size(); i++)
        {
            const auto prime = primes[i];
            if( solutions[prime] > input )
            {
                std::cout << "found solution at " << prime << "(" << i+1 << ")=" << solutions[prime];
                std::cout << " started at " << i_start << "(" << (i - i_start) << ")";
                if( out_p < out_to_validate.size() && out_to_validate[out_p++] == solution_indices[prime] ) std::cout << " PASS" <<std::endl;
                else{
                    if( out_p < out_to_validate.size() )
                    std::cout << " FAIL " << out_to_validate[out_p-1] << " " << solution_indices[prime] <<std::endl;
                }
                break;
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        // std::cout << "TOOK in total "<<  std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "mus" << std::endl;
    }

    return 0;
}

int main()
{
    do_main();
    return 0;
}