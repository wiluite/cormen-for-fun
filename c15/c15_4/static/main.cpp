#include <iostream>
#include <cstring> // runtime strcmp()

#include <boost/mp11.hpp>

using boost::mp11::mp_list;
using boost::mp11::mp_replace_at_c;
using boost::mp11::mp_size_t;
using boost::mp11::mp_at_c;
using boost::mp11::mp_plus;
using boost::mp11::mp_reverse;
using boost::mp11::mp_less;
using boost::mp11::mp_repeat_c;
using boost::mp11::mp_append;

template<int I> using mp_char = std::integral_constant<char, I>;

template <class C>
constexpr size_t seq_size(C)
{
    auto seq_ = C::value();
    int idx = 0;
    for (; seq_[idx++] !='\0'; ){}
    return idx-1;
}

template <class C1, class C2>
constexpr size_t linear_b_c_table_size(C1, C2)
{
    return (seq_size(C1{})+1)*(seq_size(C2{})+1);
}

template <class Seq2>
constexpr size_t indices_to_linear(size_t i, size_t j, Seq2)
{
    return (i)*(seq_size(Seq2{})+1)+j;
}

template <size_t LinearIndex, class Seq2>
constexpr auto linear_to_indices(Seq2)
{
    using i_type = mp_size_t<LinearIndex / (seq_size(Seq2{})+1)>;
    using j_type = mp_size_t<LinearIndex % (seq_size(Seq2{})+1)>;
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
        constexpr auto w = linear_to_indices<beg>(seq2);
        constexpr auto i = w.first;
        constexpr auto j = w.second;

        if constexpr ((i>0) && (i <= seq_size(seq1)) && (j>0) && (j <= seq_size(seq2)))
        {
            using c_type = std::decay_t<decltype(cc_)>;
            using b_type = std::decay_t<decltype(bb_)>;

            if constexpr (Seq1::value()[i-1] == Seq2::value()[j-1])
            {
                using new_cc = mp_replace_at_c<c_type,  beg, mp_plus<mp_at_c<c_type, indices_to_linear(i - 1,
                        j - 1, seq2)>, mp_size_t<1>>>;
                using new_bb = mp_replace_at_c<b_type,  beg, mp_char<'\\'>>;
                return LCS_Length<beg + 1, end>(new_bb(), new_cc(), seq1, seq2);
            }
            else
            {
                using elem1 = mp_at_c<c_type, indices_to_linear(i - 1, j, seq2)>;
                using elem2 = mp_at_c<c_type, indices_to_linear(i, j - 1, seq2)>;

                if constexpr (!mp_less<elem1, elem2>::value)
                {
                    using new_cc = mp_replace_at_c<c_type,  beg, mp_at_c<c_type, indices_to_linear(i - 1, j, seq2)>>;
                    using new_bb = mp_replace_at_c<b_type,  beg, mp_char<'|'>>;
                    return LCS_Length<beg + 1, end>(new_bb(), new_cc(), seq1, seq2);
                }
                else
                {
                    using new_cc = mp_replace_at_c<c_type,  beg, mp_at_c<c_type, indices_to_linear(i, j - 1, seq2)>>;
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
        if constexpr(std::is_same_v<mp_at_c<b_table_type, indices_to_linear(i, j, seq2)>, mp_char<'\\'>>)
        {
            using result_list_type = std::decay_t<decltype(LCS_mangled)>;
            return print_LCS_mangled<i-1,j-1>(b, mp_append<result_list_type, mp_list<mp_char<Seq1::value()[i-1]>>>(), seq1, seq2);
        } else
        if constexpr(std::is_same_v<mp_at_c<b_table_type, indices_to_linear(i, j, seq2)>, mp_char<'|'>>)
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
    using b_table = mp_repeat_c<mp_list<mp_char<' '>>, linear_b_c_table_size(s1, s2)>;
    using c_table = mp_repeat_c<mp_list<mp_size_t<0>>, linear_b_c_table_size(s1, s2)>;
    using LCS_Length_type = decltype(LCS_Length<0, linear_b_c_table_size(s1, s2)>(b_table(), c_table(), s1, s2));
    return print_LCS_mangled<seq_size(s1),seq_size(s2)>(LCS_Length_type().first, mp_list<>(), s1, s2);
}


using boost::mp11::mp_for_each;
using boost::mp11::mp_size;

template <class List>
struct mplist_to_char_const
{
    class apply
    {
    public:
        constexpr explicit apply(int value) : s(value), arr{0} {}

        constexpr operator char const *() const
        {
            return arr.data();
        }

        template <class C>
        constexpr void operator()( C ) { *(arr.data()+s) = C::value; s += 1; }

    private:
        int s;
        std::array<char, mp_size<List>::value + 1> arr;
    };

    constexpr decltype(auto) operator()() const
    {
        return mp_for_each<List>( apply(0));
    }
};

constexpr bool str_eq(char const* const s, char const* const t)
{
    return (*s == *t) && (*s == 0 || str_eq(s + 1, t + 1));
}

struct SEQUENCE_1 {constexpr static auto value() {return "0A00B000FB00";}};
struct SEQUENCE_2 {constexpr static auto value() {return "OOAOOOBFOOOBOOO";}};
using result_mp_list = decltype(LCS_mangled(SEQUENCE_1{}, SEQUENCE_2{}));
static_assert(std::is_same_v<result_mp_list, mp_list<mp_char<'A'>, mp_char<'B'>,  mp_char<'F'>,  mp_char<'B'>>>);
constexpr auto result = mplist_to_char_const<result_mp_list>().operator()();
static_assert(str_eq(result,"ABFB"));

int main()
{
    assert(strcmp(result, "ABFB") == 0);
    std::cout << result << std::endl;
    return 0;
}
