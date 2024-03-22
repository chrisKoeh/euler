#include <vector>
#include <array>
#include <iostream>
#include <chrono>

using namespace std::chrono;

constexpr int N_MAX = 2.1e3;
constexpr int A_MAX = 2e6;

std::array<int, N_MAX> non_trivial_count = {};
std::array<std::array<int, N_MAX>, N_MAX> rect_counts = {};

int find_non_trivials(int size)
{
    int sum = 0;
    for( int i = 1; i < size; i++ ) sum+= size - i;
    return sum;
}

int count_rects(int x, int y)
{
    int total_sum = 0;
    const int x_sum = non_trivial_count[x];
    const int y_sum = non_trivial_count[y];
    total_sum += (x*y);
    total_sum += (x*y_sum);
    total_sum += (y*x_sum);
    total_sum += (y_sum*x_sum);
    return total_sum;
}

int find_area(int rect_count)
{
    auto start = high_resolution_clock::now();
    int last_i = 0;
    int last_j = 0;
    int last_diff = 1e7;
    for( int i = 1; i < N_MAX; i++ )
    {
        const auto rect_count_i = rect_counts[i];
        const auto diff_ii = rect_count_i[i] - rect_count;
        if( diff_ii > last_diff ) break;

        for( int j = i; j < N_MAX; j++ )
        {
            const auto rect_count_ij = rect_count_i[j];
            const auto diff = std::abs(rect_count_ij - rect_count);
            if( diff == last_diff )
            {
                if( i * j > last_i * last_j )
                {
                    std::cout << "found " << diff << " at " << i << ", " << j << std::endl; 
                    last_i = i;
                    last_j = j;
                }
            }
            else if( diff < last_diff )
            {
                std::cout << "found " << diff << " at " << i << ", " << j << std::endl; 
                last_i = i;
                last_j = j;
                last_diff = diff;
            }
            if( rect_count_ij > rect_count ) break;
        }
    }
    auto stop = high_resolution_clock::now();
    std::cout << "Took " << duration_cast<microseconds>(stop - start).count() << "mus." << std::endl;
    return last_i * last_j;
}

int main()
{
    for(int i = 1; i < N_MAX; i++) non_trivial_count[i] = find_non_trivials(i);
    for( int i = 1; i < N_MAX; i++ )
    {
        auto & rect_count_i = rect_counts[i];
        for( int j = 1; j < N_MAX; j++ )
        {
            auto& rect_count_ij = rect_count_i[j];
            if( rect_count_ij != 0 ) continue;
            rect_count_ij = count_rects(i,j);
            if( rect_count_ij > A_MAX ) break;
            if( i != j ) rect_counts[j][i] = rect_count_ij;
        }
    }
    /*std::cout << rect_counts[1][1] << std::endl;
    std::cout << rect_counts[1][2] << std::endl;
    std::cout << rect_counts[1][3] << std::endl;
    std::cout << rect_counts[3][2] << std::endl;
    std::cout << rect_counts[2][1] << std::endl;
    std::cout << rect_counts[3][3] << std::endl;
    std::cout << rect_counts[4][1] << std::endl;
    std::cout << rect_counts[4][3] << std::endl;*/
    std::cout << find_area(18) << std::endl;
    std::cout << find_area(2) << std::endl;
    // std::cout << find_area(1e6) << std::endl;
    std::cout << find_area(60) << std::endl;
    return 0;
}