#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <map>
#include <chrono>

#include "BigInt.h"

using namespace PositiveBigInt;
using namespace std::chrono;

#define PRIME_MAX 1000

std::map<int, BigInt> solutions;
std::vector<int> primes = {2,3,5,7};
std::vector<int> squares = {};

bool is_prime(long long num) {
    if(num < 2) return false;
    for(const auto &val:primes) {
        if(val > num / val) return true;
        if(num % val == 0) return false;
    }
    return true;
}

void find_primes_to_n() {
    for(int i = 9; i < PRIME_MAX; i+=2) {
        if(is_prime(i)) primes.push_back(i);
    }
}

void find_squares_to_n(int N)
{
    for(int i = 2; i <= N; i++)
    {
        const int sq = i*i;
        squares.push_back( sq );
        if(sq > N) break;
    }
}

bool is_perfect( int n )
{
    int sq_rt = sqrt(n);
    return sq_rt * sq_rt == n;
}

BigInt* get_value_from_solutions(int num)
{
    auto pos = solutions.find(num);
    if (pos != solutions.end()) {
        return &pos->second;
    }
    return nullptr;
}

std::pair<int,int> get_square_factor( int n )
{
    std::pair<int,int> res = std::make_pair(-1,-1);
    for( const auto& sq: squares )
    {
        const int div_cache = n / sq;
        if( div_cache * sq == n )
        {
            res = std::make_pair(div_cache, sqrt(sq));
        }
        if( div_cache <= 1 ) break;
    }
    return res;
}

void sqrt_new(const int target, const int P)
{
    int start_num = sqrt( target );
    BigInt start = BigInt(start_num*start_num);
    BigInt target_cache(target);

    int old_num = start_num;

    BigInt estimate_factor(0, 10500 / start.get_threshold_exp());
    BigInt est_factor_cache(old_num,0,50);
    BigInt estimate_factor_cmpl(0);
    uint8_t count_zero = 0;
    int realP = start_num > 9 ? P - 2: P - 1;
    int init_duration = 0;
    for(int i = 0; i < realP; i++)
    {
        // slow, 40ms for P=10000, 15ms for substraction, 9ms for *100, 8ms for *10
        //auto c_start = high_resolution_clock::now();
        target_cache -= start;
        target_cache *= 100;
        //auto c_stop = high_resolution_clock::now();
        //auto duration = duration_cast<microseconds>(c_stop - c_start);
        //init_duration += duration.count();
        estimate_factor_cmpl *= 10;
        estimate_factor_cmpl += (int)(old_num*20);

        int estimate_num(0);

        if( target_cache.get_digit_count() < 50 )
        {
            start = ( estimate_factor_cmpl + estimate_num) * estimate_num;
            while( start < target_cache )
            {
                estimate_num++;
                start = ( estimate_factor_cmpl + estimate_num) * estimate_num;
            }
            if( estimate_num > 0 )
            {
                estimate_num--;
                start = ( estimate_factor_cmpl + estimate_num) * estimate_num;
            }
        }
        else
        {
            BigInt rough_estimate = estimate_factor_cmpl.get_big_int_until(5);
            BigInt rough_target = target_cache.get_big_int_until(5);
            const int diff_digit_count = target_cache.get_digit_count() - estimate_factor_cmpl.get_digit_count();
            int rough_diff_digit_count = rough_target.get_digit_count() - rough_estimate.get_digit_count();
            while( diff_digit_count < rough_diff_digit_count )
            {
                rough_estimate *= 10;
                rough_diff_digit_count--;
            }
            while( diff_digit_count > rough_diff_digit_count )
            {
                rough_target *= 10;
                rough_diff_digit_count++;
            }
            BigInt estimated = rough_estimate;
            estimated *= estimate_num;
            while( estimated < rough_target )
            {
                estimate_num++;
                estimated = rough_estimate;
                estimated *= estimate_num;
            }

            // 7ms in total for P=10000
            estimate_num--;
            start = estimate_factor_cmpl;
            start += estimate_num;
            start *= estimate_num;
        }

        // fast, 0.1ms in total for P=10000
        old_num = estimate_num;
        est_factor_cache *= 10;
        est_factor_cache += estimate_num;
        if( est_factor_cache.num[est_factor_cache.start_offset] == 0 ) count_zero++;

        if( est_factor_cache.get_digit_count() + count_zero == est_factor_cache.get_threshold_exp() )
        {
            estimate_factor.start_offset--;
            estimate_factor += est_factor_cache;
            est_factor_cache = BigInt(0,0,50);
            count_zero = 0;
        }
    }

    // fast
    const int total_digits = est_factor_cache.get_digit_count() + count_zero;
    for( int i = 0; i < total_digits/2; i++ ) estimate_factor *= 100;
    if( total_digits % 2 == 1 ) estimate_factor *= 10;
    estimate_factor += est_factor_cache;
    solutions.insert( {target, estimate_factor});
    //std::cerr << target << " init took " << init_duration << "mus" << std::endl;
}

unsigned long long run_variant(int N, int P)
{
    solutions = {};
    unsigned long long total_sum = 0;
    for( int i = 2; i <= N; i++ )
    {
        if( is_perfect( i ) ) continue;
        const auto sqf = get_square_factor(i);
        if( sqf.first != -1 )
        {
            auto* val = get_value_from_solutions(sqf.first);
            BigInt sqrtX = *val;
            sqrtX *= sqf.second;
            total_sum += sqrtX.get_digit_sum(P);
        }
        else
        {
            sqrt_new(i, P+5);

            // 4ms in total
            total_sum += get_value_from_solutions(i)->get_digit_sum(P);
        }
    }
    return total_sum;
}

void all_unit_tests()
{
    unit_tests();
    unit_test_operator(run_variant(2, 100) == 475, true );
    unit_test_operator(run_variant(10, 10000) == 315331, true );
    unit_test_operator(run_variant(1000, 1000) == 4359087, true );
}

int main()
{
    int N = 10;
    int P = 10000;
    find_squares_to_n(1000);
    find_primes_to_n();

    // std::cout << run_variant(N, P) << std::endl;

    all_unit_tests();
    return 0;
}