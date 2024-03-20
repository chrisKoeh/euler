#include <vector>
#include <array>
#include <iostream>
#include <math.h>
#include <algorithm>

//      1000-9999 : N=4, 3.3e1 - 1e2
//    10000-99999 : N=5, 1e2 - 3.4e2

void get_squares( const int N )
{
    int times_10 = N / 2;
    if( N % 2 == 0 ) times_10 -= 1;

    int factor = 1;
    for( int i = 0; i < times_10; i++ ) factor *= 10;

    int start = factor;
    int end = 3.4*factor;
    if( N % 2 == 0 )
    {
        start = 3.3 * factor;
        end = factor * 10;
    }

    std::vector<std::pair<std::string, unsigned long>> num_digits(end - start + 1);
    int offset = 0;
    for( int i = start; i <= end; i++ )
    {
        const unsigned long sq = (unsigned long)i*i;
        const int log10_ = log10( sq );
        if( log10_ == N - 1 )
        {
            auto sqs = std::to_string(sq);
            std::sort(sqs.begin(), sqs.end());
            num_digits[offset++] = std::make_pair(sqs, sq);
        }
        else if( log10_ > N - 1 )
        {
            break;
        }
    }
    // num_digits.resize(offset);

    std::sort(num_digits.begin(), num_digits.begin() + offset);
    std::vector<unsigned long> solutions;
    std::string last;
    std::vector<unsigned long> max_solutions;
    unsigned long current_max_value = 0;
    unsigned long total_max_value = 0;
    for( int i = 0; i < offset; i++ )
    {
        const auto& nd = num_digits[i];
        if( nd.first == last )
        {
            solutions.push_back( nd.second );
            if( nd.second > current_max_value ) current_max_value = nd.second;
        }
        else
        {
            if( solutions.size() > max_solutions.size() )
            {
                total_max_value = current_max_value;
                max_solutions = solutions;
            }
            solutions.clear();
            current_max_value = nd.second;
            last = nd.first;
        }
    }
    std::cout << total_max_value << std::endl;
}

int main()
{
    for( int N=13; N <=13; N++ )
        get_squares(N);
    return 0;
}