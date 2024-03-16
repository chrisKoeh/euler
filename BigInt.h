
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm> 

namespace PositiveBigInt{
    class BigInt
    {
        static constexpr unsigned long long threshold = 1e8;
        public:
            BigInt( unsigned long long n, int s_offset = -1, unsigned long long digit_count = 11000 )
            : threshold_exp(get_threshold_exp())
            , num(std::vector<unsigned long long>(digit_count / threshold_exp))
            , digit_count(digit_count)
            , start_offset(s_offset != -1 ? s_offset : 0)
            , end_offset(start_offset)
            , start_threshold(threshold)
            , effective_threshold(threshold)
            {
                while( n / threshold > 0 )
                {
                    num[end_offset++] = n % threshold;
                    n /= threshold;
                }
                num[end_offset++] = n;
            }

            BigInt operator*(unsigned long long factor)
            {
                BigInt target = *this;
                target.multiply_factor(factor);
                return target;
            }

            BigInt operator*(const BigInt& factor)
            {
                BigInt target = *this;
                target *= factor;
                return target;
            }

            void operator*=(const BigInt& factor)
            {
                BigInt original = *this;
                BigInt _this = BigInt(0, original.start_offset, original.digit_count);
                for( int i = factor.start_offset; i < factor.end_offset; i++ )
                {
                    BigInt cache = original;
                    cache.multiply_factor(factor.num[i], i - factor.start_offset);
                    _this += cache;
                }
                *this = _this;
            }

            void operator*=(unsigned long long factor)
            {
                multiply_factor(factor);
            }

            BigInt operator+(const BigInt& summand)
            {
                BigInt target = *this;
                target.add_big_int(summand);
                return target;
            }

            BigInt operator+(unsigned long long summand)
            {
                BigInt target = *this;
                target.effective_threshold = start_threshold;
                unsigned long long keep = target.add_num_to_element_primitive(summand, start_offset);
                finalize_keep(keep, start_offset+1);
                target.effective_threshold = threshold;
                return target;
            }

            void operator+=(const unsigned long long summand)
            {
                effective_threshold = start_threshold;
                unsigned long long keep = add_num_to_element_primitive(summand, start_offset);
                effective_threshold = threshold;
                finalize_keep(keep, start_offset+1);
            }

            void operator+=(const BigInt& summand)
            {
                add_big_int(summand);
            }

            BigInt operator-(const BigInt& substract)
            {
                BigInt target = *this;
                target.substract_big_int(substract);
                return target;
            }

            void operator-=(const BigInt& substract)
            {
                substract_big_int(substract);
            }

            bool operator==(const BigInt& equal)
            {
                return num == equal.num;
            }

            bool operator>(const BigInt& smaller)
            {
                if( (end_offset - start_offset) != (smaller.end_offset - smaller.start_offset ) )
                    return (end_offset - start_offset) > (smaller.end_offset - smaller.start_offset );

                const int offset_diff = start_offset - smaller.start_offset;
                for( int i = end_offset - 1; i >= start_offset; i-- )
                {
                    const auto num_i = num[i];
                    const auto smaller_num_i = smaller.num[i - offset_diff];
                    if(num_i != smaller_num_i) return num_i > smaller_num_i;
                }
                return false;
            }

            bool operator>=(const BigInt& smaller_equal)
            {
                return *this == smaller_equal || *this > smaller_equal;
            }

            bool operator<(const BigInt& larger)
            {
                return !(*this > larger) && !(*this == larger);
            }

            BigInt get_big_int_until(size_t reverse_index, size_t new_digit_count = 250) const
            {
                BigInt r(0, 0, new_digit_count);
                int start_from = end_offset - 1 - reverse_index;
                if( reverse_index >= end_offset - 1 - start_offset )
                {
                    start_from = start_offset;
                    r.start_threshold = start_threshold;
                }
                r.end_offset = r.start_offset;
                for(int i = start_from; i < end_offset; i++)
                {
                    r.num[i - start_from + r.start_offset] = num[i];
                    r.end_offset++;
                }
                return r;
            }

            unsigned long long modulo(unsigned long long m)
            {
                unsigned long long res = 0;
                res += num[start_offset] % m;
                unsigned long long total_thres = threshold % m;
                for( int i = start_offset + 1; i < end_offset; i++  )
                {
                    res += ((num[i] % m )*total_thres) % m;
                    res %= m;
                    total_thres *= (threshold%m);
                    total_thres %= m;
                }
                return res;
            }

            unsigned long long to_ull()
            {
                if( (end_offset - start_offset)*threshold_exp > 20 ) std::cout << "warning, unsigned long long might be too small" << std::endl;
                unsigned long long res = num[start_offset];
                unsigned long long thres_total = threshold;
                for( int i = start_offset + 1; i < end_offset; i++ )
                {
                    res += (thres_total*num[i]);
                    thres_total *= threshold;
                }
                return res;
            }

            int get_digit_sum(int until = 0)
            {
                int count_digits = 0;
                int digit_sum = 0;
                for( int i = end_offset - 1; i >= start_offset; i-- )
                {
                    std::string s = std::to_string(num[i]);
                    const auto effective_threshold_exp = (i == start_offset) ? get_threshold_exp(start_threshold) : threshold_exp;
                    while( s.length() < effective_threshold_exp && i != end_offset - 1 ) s = "0" + s;
                    for( int i = 0; i < s.length(); i++ )
                    {
                        if( until != 0 && count_digits >= until ) return digit_sum;
                        count_digits++;
                        digit_sum += s[i] - 48;
                    }
                }
                return digit_sum;
            }

            int get_digit_count()
            {
                int count_digits = 0;
                const int thres_exp = threshold_exp;
                if( end_offset > start_offset + 1 ) count_digits = (threshold_exp * (end_offset - 2 - start_offset));
                count_digits += std::to_string(num[end_offset - 1]).length();
                if( end_offset > start_offset + 1 ) count_digits += get_threshold_exp(start_threshold);
                return count_digits;
            }

            friend std::ostream& operator<<(std::ostream& os, const BigInt& bi)
            {
                os << bi.get_as_string();
                return os;
            }

            std::string get_as_string() const
            {
                std::stringstream os;
                if( end_offset - start_offset >= 1 )
                {
                    os << num[end_offset - 1];
                }

                for( int i = end_offset - 2; i >= start_offset + 1; i-- ) os << std::setfill('0') << std::setw(threshold_exp) << num[i];
                if( end_offset > start_offset + 1 ) os << std::setfill('0') << std::setw(get_threshold_exp(start_threshold)) << num[start_offset];
                return os.str();
            }

            unsigned long long threshold_exp;
            std::vector<unsigned long long> num;
            size_t digit_count;
            int start_offset;
            int end_offset;
            unsigned long long start_threshold;
            unsigned long long effective_threshold;

            unsigned long long get_threshold_exp(const unsigned long long thres = threshold) const
            {
                unsigned long long threshold_exp = 1;
                unsigned long long threshold_cache = thres;
                while( (unsigned long long)threshold_cache / (unsigned long long)10 > 1 )
                {
                    threshold_exp++;
                    threshold_cache /= (unsigned long long)10;
                }
                return threshold_exp;
            }

            void multiply_by_10()
            {
                if( start_threshold == threshold )
                {
                    start_offset--;
                    start_threshold = 10;
                    num[start_offset] = 0;
                    return;
                }
                else
                {
                    start_threshold *= 10;
                    num[start_offset] *= 10;
                    num[start_offset] %= start_threshold;
                }
            }

            private:
                unsigned long long add_num_to_element_primitive(const unsigned long long& n, const int index)
                {
                    unsigned long long& numi = num[index];
                    numi += n;
                    const unsigned long long res = numi / effective_threshold;
                    numi %= effective_threshold;
                    return res;
                }

                void finalize_keep(unsigned long long& keep, int start_from)
                {
                    while( keep != 0 )
                    {
                        keep = add_num_to_element_primitive(keep, start_from);
                        if( start_from++ >= end_offset ) end_offset++;
                    }
                }

                void add_big_int(const BigInt& bb)
                {
                    auto& a = num;
                    const auto& b = bb.num;
                    const int b_size = bb.end_offset;
                    unsigned long long keep = 0;
                    for( int i = bb.start_offset; i < b_size; i++ )
                    {
                        const int aind = i + start_offset - bb.start_offset;
                        const unsigned long long bi = b[i];
                        keep = add_num_to_element_primitive(bi + keep, aind);
                        if( aind >= end_offset )
                        {
                            end_offset++;
                        }
                    }
                    finalize_keep(keep, b_size + start_offset - bb.start_offset);
                }

                void substract_big_int(const BigInt& bb)
                {
                    auto& a = num;
                    const auto& b = bb.num;
                    const int b_size = bb.end_offset;
                    uint8_t keep = 0;
                    const int start_offset_diff = start_offset - bb.start_offset;
                    unsigned long long *ai = &a[start_offset];
                    if( start_threshold <= bb.start_threshold )
                    {
                        const unsigned long long thres_keep_factor = (threshold / bb.start_threshold) * start_threshold;
                        unsigned long long keep_start_thres_delay = b[bb.start_offset] / start_threshold;
                        const unsigned long long thres_factor = (threshold / thres_keep_factor);
                        *ai += start_threshold - (b[bb.start_offset] % start_threshold);
                        keep = *ai < start_threshold ? 1 : 0;
                        *ai %= start_threshold;
                        for( int i = bb.start_offset + 1; i < b_size; i++ )
                        {
                            ai = &a[i + start_offset_diff];
                            const unsigned long long& bi = b[i];
                            unsigned long long effective_bi = bi % thres_keep_factor;
                            effective_bi *= thres_factor;
                            effective_bi += keep_start_thres_delay;
                            keep_start_thres_delay = bi / thres_keep_factor;
                            *ai += threshold - effective_bi - keep;
                            keep = *ai < threshold ? 1 : 0;
                            *ai %= threshold;
                        }
                        ai = &a[b_size + start_offset_diff];
                        *ai += threshold;
                        *ai -= (keep_start_thres_delay + keep);
                        keep = *ai < threshold ? 1 : 0;
                        *ai %= threshold;
                        ai++;
                    }
                    else
                    {
                        const unsigned long long thres_div = start_threshold / bb.start_threshold;
                        unsigned long long effective_bi = b[bb.start_offset];
                        const int bb_offset_diff = bb.end_offset - bb.start_offset;

                        if( bb_offset_diff > 1 ) effective_bi += ((b[bb.start_offset + 1] % thres_div) * bb.start_threshold);
                        *ai += start_threshold - effective_bi;
                        keep = *ai < start_threshold ? 1 : 0;
                        *ai %= start_threshold;
                        for( int i = bb.start_offset + 1; i < b_size; i++ )
                        {
                            ai = &a[i + start_offset_diff];
                            effective_bi = b[i] / thres_div;
                            if( i < bb.end_offset - 1 ) effective_bi += ((b[i + 1] % thres_div) * (threshold / thres_div));
                            *ai += threshold - ( effective_bi + keep);
                            keep = *ai < threshold ? 1 : 0;
                            *ai %= threshold;
                        }
                    }
                    while( keep == 1 )
                    {
                        *ai += threshold - 1;
                        keep = *ai < threshold ? 1 : 0;
                        *ai %= threshold;
                    }
                    while( a[end_offset - 1] == 0 && end_offset - start_offset > 1 ) end_offset--;
                }

                void multiply_factor(const unsigned long long& factor, const int offset = 0)
                {
                    auto& a = num;
                    if( factor == 0 )
                    {
                        end_offset = start_offset;
                        a[end_offset++] = 0;
                        return;
                    }

                    if( factor == 1 && offset == 0 ) return;

                    if( offset != 0 )
                    {
                        BigInt cache(0, this->start_offset, this->digit_count);
                        cache.end_offset = offset + end_offset;
                        cache.start_offset = start_offset;
                        unsigned long long keep = 0;
                        for( int i = start_offset; i < end_offset; i++ )
                        {
                            const unsigned long c = (num[i] * factor) + keep;
                            keep = cache.add_num_to_element_primitive( c, i + offset);
                        }
                        cache.finalize_keep( keep, cache.end_offset);
                        *this = cache;
                    }
                    else
                    {
                        unsigned long long keep = 0;
                        effective_threshold = start_threshold;
                        keep = add_num_to_element_primitive( num[start_offset]*(factor-1), start_offset);
                        effective_threshold = threshold;
                        for( int i = start_offset + 1; i < end_offset; i++ )
                        {
                            const unsigned long long c = (num[i] * (factor - 1)) + keep;
                            keep = add_num_to_element_primitive( c, i + offset);
                        }
                        finalize_keep( keep, end_offset);
                    }
                }
    };

    void unit_test(BigInt a, const std::string& expected )
    {
        std::string printed_a = a.get_as_string();
        const int width = 60;
        if( printed_a.size() > 50 )
        {
            printed_a = std::string(printed_a.begin(), printed_a.begin() + width - 21);
            printed_a += "...(digits=" + std::to_string(a.get_digit_count()) + ")";
        }
        std::stringstream ss;
        ss << printed_a;
        std::cout << "TEST a=" << printed_a << " " << std::setfill(' ') << std::setw(width - ss.str().length());
        if(a.get_as_string() == expected ) std::cout << "PASS" << std::endl;
        else std::cout << "FAIL" << std::endl;
    }

    void unit_test_operator(bool result, bool expected = true )
    {
        std::cout << "TEST operator " << std::setfill(' ') << std::setw(54);
        if(result == expected ) std::cout << "PASS" << std::endl;
        else std::cout  << "FAIL" << std::endl;
    }

    void unit_tests()
    {
        BigInt a(0);
        BigInt b(10);

        unit_test(a, "0");
        a = a+b;
        unit_test(a, "10");
        a = a * BigInt(1000);
        unit_test(a, "10000");
        a = a - b;
        unit_test(a, "9990");
        a = a * 8;
        unit_test(a, "79920");
        BigInt c(79900);
        a = a - c;
        unit_test(a, "20");
        a = a * 7777;
        unit_test(a, "155540");
        a = a * 6666;
        unit_test(a, "1036829640");
        BigInt d(1036829640);
        a = a - d;
        unit_test(a, "0");
        a = a + d;
        unit_test(a, "1036829640");
        BigInt e(9640);
        a = a - e;
        unit_test(a, "1036820000");
        BigInt f(5);
        a = a - f;
        unit_test(a, "1036819995");
        a += 0;
        unit_test(a, "1036819995");
        a -= 0;
        unit_test(a, "1036819995");
        f += 100000;
        unit_test(f, "100005");
        f += e;
        unit_test(f, "109645");
        f -= e;
        unit_test(f, "100005");
        f *= 10;
        unit_test(f, "1000050");
        f *= 20;
        unit_test(f, "20001000");

        BigInt o = f;
        unit_test_operator(o > f, false);
        unit_test_operator(o == f);
        o += 1;
        unit_test_operator(o > f);
        unit_test_operator(o < f, false);
        unit_test_operator(a > f);
        unit_test_operator(a > o);
        o -= 1;
        unit_test_operator(o == f);
        unit_test_operator(o < f, false);
        unit_test_operator(o > f, false);
        unit_test_operator(f > o, false);

        a *= 0;
        unit_test(a, "0");
        BigInt m(9999);
        BigInt n(9999);
        m *= n;
        unit_test(m, "99980001");
        m *= n;
        unit_test(m, "999700029999");
        BigInt s(9900);
        BigInt t(9901);
        s*=t;
        unit_test(s, "98019900");
        BigInt u(999099);
        BigInt v(99901);
        u *= v;
        unit_test(u, "99810989199");
        BigInt from(99999999999);
        BigInt split_2 = from.get_big_int_until(1);
        unit_test(split_2, "9999999");
        BigInt split_4 = from.get_big_int_until(2);
        unit_test(split_4, "99999999999");
        BigInt split_5 = from.get_big_int_until(5);
        unit_test(split_5, "99999999999");
        BigInt split_10 = from.get_big_int_until(10);
        unit_test(split_10,"99999999999");

        BigInt a1(99009900);
        BigInt a2(990099);
        a1 *= a2;
        unit_test(a1, "98029602980100");
        a1 = BigInt(99009900);
        a2 = BigInt(990099);
        a2 *= a1;
        unit_test(a2, "98029602980100");
        BigInt mult(990100);
        BigInt fact(990100);
        mult *= fact;
        unit_test(mult, "980298010000");
        unit_test_operator(mult.get_digit_count() == 12);
        unit_test_operator(mult.get_digit_sum(3) == 17);
        unit_test_operator(mult.get_digit_sum(5) == 28);
        unit_test_operator(mult.get_digit_sum(6) == 36);
        unit_test_operator(mult.get_digit_sum(9) == 37);
        BigInt a_off(1);
        BigInt a_off11(0);
        a_off11.start_offset = 11;
        a_off11.end_offset = 12;
        a_off11 += 5;
        unit_test(a_off11, "5");
        a_off += a_off11;
        unit_test(a_off, "6");
        a_off -= 4;
        unit_test(a_off, "2");
        a_off11 -= 3;
        unit_test(a_off11, "2");
        a_off11 = a_off11 + a_off;
        unit_test(a_off11, "4");
       a_off11 *= a_off;
        unit_test(a_off11, "8");
        a_off11 *= 87;
        unit_test(a_off11, "696");
        a_off *= 87;
        unit_test(a_off, "174");
        a_off11 *= a_off;
        unit_test(a_off11, "121104");
        a_off *= 100;
        unit_test(a_off, "17400");

        // performance tests
        BigInt perf(1);
        std::string s_perf = "1";
        for( int i = 0; i < 10000; i++ )
        {
            perf *= 10;
            s_perf.push_back(48);
        }
        unit_test(perf, s_perf);

        perf = BigInt(1);
        s_perf = "1";
        srand((unsigned) time(NULL));
        for( int i = 0; i < 1000; i++ )
        {
            int random = rand() % 10;
            perf *= 10;
            perf += random;
            s_perf.push_back( random + 48 );
        }
        unit_test( perf, s_perf );

        // start_threshold test
        BigInt start_thres = BigInt(1, 5);
        start_thres.multiply_by_10();
        start_thres += 15;
        unit_test( start_thres, "25" );
        unit_test_operator( start_thres.get_digit_count() == 2 );
        BigInt new_start_thres = start_thres.get_big_int_until(1);
        unit_test(new_start_thres, "25");
        new_start_thres = start_thres.get_big_int_until(2);
        unit_test(new_start_thres, "25");
        start_thres.multiply_by_10();
        start_thres += 27;
        unit_test( start_thres, "277" );
        unit_test_operator( start_thres.get_digit_count() == 3 );
        unit_test_operator( start_thres.get_digit_sum() == 16 );
        start_thres.multiply_by_10();
        unit_test( start_thres, "2770" );
        unit_test_operator( start_thres.get_digit_count() == 4 );
        unit_test_operator( start_thres.get_digit_sum() == 16 );
        start_thres.multiply_by_10();
        unit_test( start_thres, "27700" );
        start_thres += 1;
        unit_test_operator( start_thres.get_digit_count() == 5 );
        unit_test_operator( start_thres.get_digit_sum(2) == 9 );
        unit_test_operator( start_thres.get_digit_sum(4) == 16 );
        start_thres.multiply_by_10();
        start_thres.multiply_by_10();
        unit_test_operator( start_thres.get_digit_count() == 7 );
        unit_test_operator( start_thres.get_digit_sum() == 17 );
        unit_test( start_thres, "2770100" );
        start_thres+=15;
        unit_test_operator( start_thres.get_digit_count() == 7 );
        unit_test_operator( start_thres.get_digit_sum(2) == 9 );
        unit_test_operator( start_thres.get_digit_sum(3) == 16 );
        unit_test_operator( start_thres.get_digit_sum(4) == 16 );
        unit_test_operator( start_thres.get_digit_sum(4) == 16 );
        unit_test_operator( start_thres.get_digit_sum(5) == 17 );
        unit_test_operator( start_thres.get_digit_sum(6) == 18 );
        unit_test_operator( start_thres.get_digit_sum(7) == 23 );
        unit_test( start_thres, "2770115" );
        BigInt subst(440);
        start_thres -= subst;
        unit_test( start_thres, "2769675");
        start_thres.multiply_by_10();
        unit_test( start_thres, "27696750");
        start_thres -= BigInt(6660);
        unit_test( start_thres, "27690090");
        start_thres -= BigInt(690046);
        unit_test( start_thres, "27000044");
        start_thres -= BigInt(5000);
        unit_test( start_thres, "26995044");
        start_thres -= BigInt(23982437);
        unit_test( start_thres, "3012607");
        start_thres -= BigInt(2648003);
        unit_test( start_thres, "364604");

        start_thres = BigInt(25, 5);
        start_thres.multiply_by_10();
        start_thres *= 3;
        unit_test( start_thres, "750");
        start_thres.multiply_by_10();
        start_thres *= 50;
        unit_test( start_thres, "375000");

        start_thres = BigInt(564123, 5);
        BigInt subst_thres = BigInt(3567, 5);
        start_thres.multiply_by_10();
        start_thres.multiply_by_10();
        start_thres += 22;
        unit_test( start_thres, "56412322");
        subst_thres.multiply_by_10();
        subst_thres.multiply_by_10();
        subst_thres.multiply_by_10();
        subst_thres += 833;
        unit_test( subst_thres, "3567833");
        start_thres -= subst_thres;
        unit_test( start_thres, "52844489");
        subst_thres = BigInt(1224, 5);
        subst_thres.multiply_by_10();
        subst_thres.multiply_by_10();
        subst_thres += 55;
        start_thres -= subst_thres;
        unit_test(start_thres, "52722034");

        start_thres = BigInt(5645, 5);
        start_thres.multiply_by_10();
        start_thres.multiply_by_10();
        start_thres.multiply_by_10();
        unit_test(start_thres, "5645000");
        subst_thres = BigInt(4335, 5);
        subst_thres.multiply_by_10();
        subst_thres.multiply_by_10();
        start_thres -= subst_thres;
        unit_test(start_thres, "5211500");
        subst_thres = BigInt(511140, 5);
        subst_thres.multiply_by_10();
        start_thres -= subst_thres;
        unit_test(start_thres, "100100");
        start_thres = BigInt(5645,5);
        start_thres.multiply_by_10();
        start_thres.multiply_by_10();
        start_thres += 5;
        subst_thres = BigInt(4444, 5);
        subst_thres.multiply_by_10();
        subst_thres += 15;
        unit_test(start_thres, "564505");
        unit_test(subst_thres, "44455");
        start_thres -= subst_thres;
        unit_test(start_thres, "520050");
        subst_thres *=9;
        unit_test(subst_thres, "400095");
        start_thres = BigInt(44445555);
        subst_thres = BigInt(4438, 5);
        subst_thres.multiply_by_10();
        start_thres -= subst_thres;
        unit_test(start_thres, "44401175");
        subst_thres += 9;
        subst_thres.multiply_by_10();
        start_thres -= subst_thres;
        unit_test(start_thres, "43957285");
        subst_thres += 180;
        subst_thres.multiply_by_10();
        unit_test(subst_thres, "4440700");
        start_thres -= subst_thres;
        unit_test(start_thres, "39516585");
        start_thres += BigInt((unsigned long long)10000000);
        unit_test(start_thres, "49516585");
        subst_thres += 25;
        subst_thres *= 10;
        start_thres -= subst_thres;
        unit_test(subst_thres, "44407250");
        unit_test(start_thres, "5109335");

        BigInt mod_(1111234);
        unit_test_operator( mod_.modulo(3) == 1 );
        unit_test_operator( mod_.modulo(17) == 12 );
        unit_test_operator( mod_.modulo(5) == 4 );
        unit_test_operator( mod_.modulo(1124) == 722 );
        unit_test_operator( mod_.modulo(10233) == 6070 );
        unit_test_operator( mod_.to_ull() == 1111234 );

        mod_ = BigInt(5891201239012398);
        unit_test_operator( mod_.modulo(3) == 0 );
        unit_test_operator( mod_.modulo(17) == 8 );
        unit_test_operator( mod_.modulo(5) == 3 );
        unit_test_operator( mod_.modulo(1124) == 262 );
        unit_test_operator( mod_.modulo(10233) == 477 );
        unit_test_operator( mod_.to_ull() == 5891201239012398 );

        BigInt fac(12);
        fac *= fac;
        unit_test(fac, "144");
        BigInt f_(100021313);
        f_ *= 1.5e12;
        unit_test( f_, "15003196950000000000" );
        f_ *= 1.5e12;
        unit_test( f_, "2250479542500000000000000000000000");
   }
}