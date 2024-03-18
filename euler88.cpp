#include <vector>
#include <array>
#include <iostream>
#include <math.h>
#include <algorithm>

constexpr int PRIME_MAX = 5e5;
constexpr int N_MAX = 2.1e5;
constexpr uint8_t N_LOG2_MAX = 20;
constexpr uint8_t MAX_SUMS = 150;

const std::vector<int> primes_single_digit = {2,3,5,7};
const std::vector<int> factors_single_digits = {2,3,4,5,6,7,8,9};
std::vector<int> primes = primes_single_digit;
std::array<bool, PRIME_MAX> b_is_prime = {};
std::array<std::array<int, N_LOG2_MAX>, N_MAX> prime_factors = {{}};
std::array<std::array<int, N_LOG2_MAX>, N_MAX> prime_factors_with_duplicates = {{}};
std::array<std::array<int, MAX_SUMS>, N_MAX> sums = {{}};

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

std::array<int, N_LOG2_MAX> find_prime_factors(const int num)
{
    std::array<int, N_LOG2_MAX> factors;
    auto& factors_with_dupl = prime_factors_with_duplicates[num];
    int offset = 0;
    int offset_dupl = 0;
    int n = num;
    while( n > 1 )
    {
        if( b_is_prime[ n ] )
        {
            if( offset == 0 || factors[offset - 1] != n )
            {
                factors[offset++] = n;
            }
            factors_with_dupl[offset_dupl++] = n;
            break;
        }
        for( const auto& p: primes )
        {
            if( n % p == 0 )
            {
                if( offset == 0 || factors[offset - 1] != p )
                {
                    factors[offset++] = p;
                }
                factors_with_dupl[offset_dupl++] = p;
                n /=p;
                break;
            }
        }
    }

    factors_with_dupl[offset_dupl] = 0;
    factors[offset] = 0;
    return factors;
}

int get_min_sum(int num)
{
    const auto& pf = prime_factors_with_duplicates[num];
    int sum = 0;
    for( const auto& p: pf )
    {
        if( p == 0 ) break;
        sum += p - 1;
    }
    return sum;
}

// split into factor
bool check_product_sum( const int& num, const int& target_product, const int& sum, const int& last_factor)
{
    // std::cout << num << " - " << product << " - " << sum << std::endl;
    if( num == 1 ) return target_product == sum;
    if( sum >= target_product ) return false;

    if( b_is_prime[num] )
    {
        bool is_valid = (target_product == sum - 1 + num);
        if( !is_valid && last_factor > 1 )
        {
            is_valid = (target_product == sum + (last_factor * num) - last_factor);
        }
        return is_valid;
    }

    auto pfactors = prime_factors[num];
    const auto& p0 = pfactors[0];
    if( check_product_sum( num / p0, target_product, sum + p0 - 1, p0 ) ) return true;

    if( last_factor > 1 )
    {
        for( int i = 0; i < N_LOG2_MAX; i++ )
        {
            const auto pi = pfactors[i];
            if( pi == 0 ) break;
            int prod = last_factor * pi;
            const int new_sum = sum + prod - last_factor;
            if( new_sum >= target_product ) return false;
            if( check_product_sum( num / pi, target_product, new_sum, prod ) ) return true;
        }
    }

    return false;
}

bool check_product_sum( const int num, const int& start_sum)
{
    // std::cout << num << " - " << product << " - " << sum << std::endl;
    return is_in_array( sums[num], num - start_sum );
}

int main()
{
    int N = 12;
    find_primes_to_n();
    int count_nums = 0;
    for( int i = 2; i < N_MAX; i++ )
    {
        if( b_is_prime[i] ) continue;
        prime_factors[i] = find_prime_factors( i );
    }

    std::array<int, 500> last_nums = {};
    int offset_last_nums = 0;
    const int last_nums_size = last_nums.size();
    unsigned long total_sum = 0;
    int start_num;
    for( int k = 2; k <= 2e5; k++ )
    {
        int log2_ = log2(k);
        start_num = k + log2_;
        bool found = false;
        for( int i_start = start_num; i_start <= start_num + 2500 - log2_; i_start++ )
        {
            if( b_is_prime[i_start] ) continue;
            if( get_min_sum(i_start) + k > i_start ) continue;

            if( check_product_sum( i_start, i_start, k, 1) )
            {
                found = true;
                // std::cout << k << ": " << i_start << std::endl;
                if( offset_last_nums == last_nums_size ) offset_last_nums = 0;
                if( std::find(last_nums.begin(), last_nums.end(), i_start) == last_nums.end() )
                {
                    total_sum += i_start;
                    last_nums[offset_last_nums++] = i_start;
                }
                break;
            }
        }
        if( !found ) std::cout << k << " NONE FOUND!!" << std::endl;
    }
    std::cout << total_sum << std::endl;
    return 0;
}