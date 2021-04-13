#include <iostream>

#include <boost/mp11.hpp>

using boost::mp11::mp_list;
using boost::mp11::mp_replace_at_c;
using boost::mp11::mp_int;
using boost::mp11::mp_at_c;
using boost::mp11::mp_plus;
using boost::mp11::mp_reverse;
using boost::mp11::mp_less;
using boost::mp11::mp_repeat_c;
using boost::mp11::mp_append;

template<int I> using mp_char = std::integral_constant<char, I>;

template <class C>
constexpr size_t seq_size2(C)
{
    auto seq_ = C::value();
    int idx = 0;
    for (; seq_[idx++] !='\0'; ){}
    return idx-1;
}

template <class C1, class C2>
constexpr size_t linear_b_c_table_size2(C1, C2)
{
    return (seq_size2(C1{})+1)*(seq_size2(C2{})+1);
}

template <class Seq2>
constexpr size_t indices_to_linear2(size_t i, size_t j, Seq2)
{
    return (i)*(seq_size2(Seq2{})+1)+j;
}

template <size_t LinearIndex, class Seq2>
constexpr auto linear_to_indices2(Seq2)
{
    using i_type = mp_int<LinearIndex / (seq_size2(Seq2{})+1)>;
    using j_type = mp_int<LinearIndex % (seq_size2(Seq2{})+1)>;
    return std::pair{i_type(), j_type()};
}

template <int beg, int end, class Seq1, class Seq2>
constexpr auto LCS_Length(auto && bb_, auto && cc_, Seq1 seq1, Seq2 seq2)
{
    if constexpr (beg==end)
    {
        return std::pair{bb_, cc_};
    } else
    {
        constexpr auto w = linear_to_indices2<beg>(seq2);
        constexpr auto i = w.first;
        constexpr auto j = w.second;

        if constexpr ((i>0) && (i <= seq_size2(seq1)) && (j>0) && (j <= seq_size2(seq2)))
        {
            using c_type = std::decay_t<decltype(cc_)>;
            using b_type = std::decay_t<decltype(bb_)>;

            if constexpr (Seq1::value()[i-1] == Seq2::value()[j-1])
            {
                using new_cc = mp_replace_at_c<c_type,  beg, mp_plus<mp_at_c<c_type, indices_to_linear2(i - 1, j - 1, seq2)>, mp_int<1>>>;
                using new_bb = mp_replace_at_c<b_type,  beg, mp_char<'\\'>>;
                return LCS_Length<beg + 1, end>(new_bb(), new_cc(), seq1, seq2);
            }
            else
            {
                using elem1 = mp_at_c<c_type, indices_to_linear2(i - 1, j, seq2)>;
                using elem2 = mp_at_c<c_type, indices_to_linear2(i, j - 1, seq2)>;

                if constexpr (!mp_less<elem1, elem2>::value)
                {
                    using new_cc = mp_replace_at_c<c_type,  beg, mp_at_c<c_type, indices_to_linear2(i - 1, j, seq2)>>;
                    using new_bb = mp_replace_at_c<b_type,  beg, mp_char<'|'>>;
                    return LCS_Length<beg + 1, end>(new_bb(), new_cc(), seq1, seq2);
                }
                else
                {
                    using new_cc = mp_replace_at_c<c_type,  beg, mp_at_c<c_type, indices_to_linear2(i, j - 1, seq2)>>;
                    using new_bb = mp_replace_at_c<b_type,  beg, mp_char<'-'>>;
                    return LCS_Length<beg + 1, end>(new_bb(), new_cc(), seq1, seq2);
                }
            }
        } else
        {
            return LCS_Length<beg + 1, end>(bb_,cc_, seq1, seq2);
        }
    }
}

template <int i, int j, class Seq1, class Seq2>
auto print_LCS_mangled(auto && b, auto && LCS_mangled, Seq1 seq1, Seq2 seq2)
{
    if constexpr((i > 0) && (j > 0))
    {
        using b_table_type = std::decay_t<decltype(b)>;
        if constexpr(std::is_same_v<mp_at_c<b_table_type, indices_to_linear2(i, j, seq2)>, mp_char<'\\'>>)
        {
            using result_list_type = std::decay_t<decltype(LCS_mangled)>;
            return print_LCS_mangled<i-1,j-1>(b, mp_append<result_list_type, mp_list<mp_char<Seq1::value()[i-1]>>>(), seq1, seq2);
        } else
        if constexpr(std::is_same_v<mp_at_c<b_table_type, indices_to_linear2(i, j, seq2)>, mp_char<'|'>>)
        {
            return print_LCS_mangled<i-1,j>(b, LCS_mangled, seq1, seq2);
        } else
        {
            return print_LCS_mangled<i,j-1>(b, LCS_mangled, seq1, seq2);
        }
    } else
    {
        using result_list_type = std::decay_t<decltype(LCS_mangled)>;
        return mp_reverse<result_list_type>();
    }
}

template <class Seq1, class Seq2>
constexpr auto LCS_mangled(Seq1 s1, Seq2 s2)
{
    using b_table = mp_repeat_c<mp_list<mp_char<' '>>, linear_b_c_table_size2(s1, s2)>;
    using c_table = mp_repeat_c<mp_list<mp_int<0>>, linear_b_c_table_size2(s1, s2)>;
    using LCS_Length_type = decltype(LCS_Length<0, linear_b_c_table_size2(s1, s2)>(b_table(), c_table(), s1, s2));
    return print_LCS_mangled<seq_size2(s1),seq_size2(s2)>(LCS_Length_type().first, mp_list<>(), s1, s2);
}

struct SEQUENCE_1 {constexpr static auto value() {return "0A00B000FB00";}};
struct SEQUENCE_2 {constexpr static auto value() {return "OOAOOOBFOOOBOOO";}};

using result2 = decltype(LCS_mangled(SEQUENCE_1{}, SEQUENCE_2{}));
static_assert(std::is_same_v<result2, mp_list<mp_char<'A'>, mp_char<'B'>,  mp_char<'F'>,  mp_char<'B'>>>);


int main()
{
    return 0;
}
