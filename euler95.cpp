#include <vector>
#include <array>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <cassert>
#include <functional>
#include "Primes.h"


constexpr uint16_t MAX_FACTORS = 500;

std::array<int, N_MAX> factor_sums = {};

void find_factor_sum_3000(const int num)
{

    auto all_factors = find_all_distinct_factors<MAX_FACTORS>(num);

    int sum = 0;
    for( const auto f: all_factors )
    {
        if( f == 0 ) break;
        sum+= f;
    }
    factor_sums[num] = sum;
}

int main()
{
    find_primes_to_n();

    int N;
    N=2e4;
    for(int i = 2; i < N_MAX; i++)
    {
        find_factor_sum_3000(i);
    }

    // find_factor_sum_3000(97846);
    //std::cout << factor_sums[97846] << std::endl;
    // assert(factor_sums[97846] == 76394);

    // find chains
    int longest_chain = 0;
    int res = 1e7;
    for( int i = 2; i <= N; i++ )
    {
        std::vector<int> chain = {};
        int current_el = i;
        do
        {
            chain.push_back( current_el );
            current_el = factor_sums[current_el];
        } while ( std::find( chain.begin(), chain.end(), current_el ) == chain.end() && current_el <= N && current_el > 1 );

        if( current_el > 1 && current_el <= N )
        {
            const auto found = std::find( chain.begin(), chain.end(), current_el );
            const int chain_len = chain.size() - (found - chain.begin());
            // std::cout << "found chain with len " << chain_len << std::endl;
            if( chain_len >= longest_chain )
            {
                int smallest_mem = *found;
                for(auto it = found+1; it != chain.end(); it++)
                {
                    smallest_mem = std::min(*it, smallest_mem);
                }
                if( chain_len > longest_chain ) res = smallest_mem;
                else
                {
                    // std::cout << "CHAIN FOUND SAME LEN ";
                    // std::cout << std::endl;
                    res = std::min(res, smallest_mem);
                }
                longest_chain = chain_len;
            }
        }
    }
    std::cout << res << std::endl;
    return 0;
}