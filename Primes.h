#include <vector>
#include <array>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <cassert>
#include <functional>


constexpr int N_MAX = 1e6 + 1;
constexpr int PRIME_MAX = N_MAX;

std::vector<int> primes = {2,3,5,7};
std::array<bool, PRIME_MAX> b_is_prime = {};

bool is_prime(unsigned long long num) {
    if(num < 2) return false;
    for(const auto &val:primes) {
        if(val > num / val) return true;
        if(num % val == 0) return false;
    }
    return true;
}

void find_primes_to_n() {
    for(unsigned long long i = 9; i < PRIME_MAX; i+=2) {
        if(is_prime(i)) primes.push_back(i);
    }
    for( const auto&p : primes ) b_is_prime[p] = true;
}

template<size_t N>
bool is_in_array(const std::array<int, N>& arr, int num)
{
    for( const auto& a: arr )
    {
        if( a == num ) return true;
        if( a == 0 ) break;
    }
    return false;
}

template<size_t N>
void print_array(const std::array<int, N>& arr, int num)
{
    int sum = 0;
    std::cout << num;
    for(auto aa: arr)
    {
        sum += aa;
        if( aa == 0 ) break;
        std::cout <<" " << aa;
    }
    std::cout << " = " << sum << std::endl;
}

template<size_t N>
std::array<int, N> find_prime_factors(const int num, bool w_duplicates)
{
    if( b_is_prime[num] )
    {
        return {};
    }

    std::array<int, N> factors;
    int offset = 0;
    int n = num;
    while( n > 1 )
    {
        if( b_is_prime[ n ] && n != num )
        {
            if( w_duplicates || offset == 0 || factors[offset - 1] != n )
            {
                factors[offset++] = n;
            }
            break;
        }
        for( const auto& p: primes )
        {
            if( n % p == 0 )
            {
                if( w_duplicates || offset == 0 || factors[offset - 1] != p )
                {
                    factors[offset++] = p;
                }
                n /=p;
                break;
            }
        }
    }

    factors[offset] = 0;
    return factors;
}

template<size_t N>
std::array<int, N> find_all_distinct_factors(const int num)
{
    auto factors = find_prime_factors<N>(num, true);
    std::array<int, N> all_factors = {};
    int all_offset = 0;
    std::function<void(const int start_offset, const int start_prod)> get_factors = [&](int start_offset, int start_prod){
        for( int i = start_offset; i  < N; i++ )
        {
            if( factors[i] == 0 ) break;
            auto prod = start_prod * factors[i];
            if( num % prod == 0 && prod < num )
            {
                // all_factors[all_offset] = 0;
                if(!is_in_array(all_factors, prod) )
                {
                    all_factors[all_offset++] = prod;
                }
                get_factors( i + 1, prod );
            }
        }
    };
    for(int i = 0; i < N; i++)
    {
        if( factors[i] == 0 ) break;
        get_factors(i+1, factors[i]);
    }
    all_factors[all_offset++] = 1;
    int last_f = -1;
    for( const auto& f: factors )
    {
        if( f == 0 ) break;
        if( f != last_f ) all_factors[all_offset++] = f;
        last_f = f;
    }
    all_factors[all_offset] = 0;
    return all_factors;
}