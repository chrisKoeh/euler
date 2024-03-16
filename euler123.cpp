#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cassert>
#include <chrono>

#include "BigInt.h"

#define PRIME_MAX 3500

using namespace PositiveBigInt;
using namespace std::chrono;

constexpr unsigned long long B_EDGE = 1e12;
constexpr unsigned long long N_MAX = 1.005e6;
std::vector<unsigned long long> primes = {2,3,5,7};
std::array<std::pair<int, unsigned long long>, N_MAX> solutions = {};

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

unsigned long long calc_remainder_primitive( int n )
{
    unsigned long long prime = primes[ n-1 ];
    unsigned long long divisor = prime * prime;
    unsigned long long prime_plus = prime + 1;
    unsigned long long prime_minus = prime - 1;
    unsigned long long prime_plus_res = 1;
    unsigned long long prime_minus_res = 1;
    for( int i = 0; i < n; i++)
    {
        prime_plus_res *= prime_plus;
        prime_minus_res *= prime_minus;
    }
    std::cout << prime_plus_res << "+" << prime_minus_res << "%" << divisor;
    unsigned long long res = (prime_plus_res + prime_minus_res) % divisor;
    std::cout << "=" << res << std::endl;
    return res;
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
            b_res *= BigInt(res, 0 , max_digits);
            res = b_res.modulo(divisor);
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

unsigned long long power_of_n_prim(const unsigned long long& a, const unsigned long long& b, const unsigned long long& divisor = 1e18)
{
    if( b == 0 ) return 1;

    int c = 1;
    int total_c = 1;
    unsigned long long res = a;
    unsigned long long factor = a;
    while( total_c < b )
    {
        unsigned long long res_ = res * factor;
        res *= factor;
        res %= divisor;

        c *= 2;
        unsigned long long factor_ = factor * factor;

        factor *= factor;
        factor %= divisor;
        total_c += c/2;
        if( c + total_c > b )
        {
            c = 1;
            factor = a;
            //factor %= divisor;
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

unsigned long long calc_remainder_prim( int n )
{
    const unsigned long long prime = primes[ n-1 ];
    const unsigned long long divisor = prime * prime;
    const unsigned long long prime_plus = prime + 1;
    const unsigned long long prime_minus = prime - 1;

    unsigned long long prime_plus_res = power_of_n_prim( prime_plus, n, divisor );
    unsigned long long prime_minus_res = power_of_n_prim( prime_minus, n, divisor );

    std::cout << prime_plus_res << "+" << prime_minus_res << "%" << divisor;
    unsigned long long res = ((prime_plus_res % divisor) + (prime_minus_res % divisor )) % divisor;
    std::cout << "=" << res << std::endl;
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

int main()
{
    find_primes_to_n();
    std::cout << primes.size() << " found" << std::endl;
    for( int n = 1; n <= 80000; n+=2 )
    {
        unsigned long long res = calc_remainder(n);
        // std::cout << n << ": " << res << std::endl;
        solutions[ primes[n-1] ] = std::make_pair(n, res);

        if( res >= B_EDGE)
        {
            std::cout << "broken at " << n << ":" << primes[n - 1] << std::endl;
            break;
        }
    }

    for(int j = 0; j < 100000; j++)
    {
        unsigned long long N = 1e12;
        for(int i = sqrt(N); i < N_MAX; i++)
        {
            if( solutions[i].second > N )
            {
                // std::cout << "found solution at " << i << ": " << solutions[i].second << " started at " << sqrt(N) << "(" << solutions[i].first << ")" << std::endl;
                break;
            }
        }
    }

    return 0;
}