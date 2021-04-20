#include <iostream>
#include <array>
#include <cassert>
#include <cstring> // strcmp()

#define CONCAT2(x, y) x##y
#define CHAR_CONST_PTR(x) struct CONCAT2(_, x)      \
{                                                   \
        constexpr static auto value() {return #x;}  \
}

namespace cormen
{
    template <class Seq>
    constexpr size_t getsize(Seq const x) noexcept
    {
        int idx = 0;
        for (; x[idx++] !='\0'; ){}
        return idx-1;
    }

    template <class Seq1, class Seq2>
    constexpr auto max_seq_size()
    {
        return std::max(getsize(Seq1::value()), getsize(Seq2::value()));
    }

    template <class Seq1, class Seq2>
    class ch15_4_subsequence
    {
    public:
        using size_type              = std::size_t;
        using bool_type              = bool;
        using class_type             = ch15_4_subsequence<Seq1, Seq2>;
        using value_type             = char;
        using reference              = value_type&;
        using const_reference        = value_type const&;
        using iterator               = value_type*;
        using const_iterator         = value_type const*;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        ch15_4_subsequence(class_type const& rhs) = delete;
        class_type& operator =(class_type const& rhs) = delete;

    private:

        const size_t m= getsize(Seq1::value());
        const size_t n= getsize(Seq2::value());

        [[nodiscard]] constexpr size_t indices_to_linear(size_t i, size_t j) const noexcept
        {
            return i * ( n + 1 ) + j;
        }

        [[nodiscard]] constexpr auto linear_to_indices(size_t linear_idx) const noexcept
        {
            return std::pair{linear_idx / (n+1), linear_idx % (n+1)};
        }

        constexpr void fill_tables(int start, int end) noexcept
        {
            if (start == end)
                return;

            auto [i, j] = linear_to_indices(start);

            if ((i>0) && (i <= m) && (j>0) && (j <= n))
            {
                if ((Seq1::value())[i-1] == (Seq2::value())[j-1])
                {
                    c[start] = c[indices_to_linear(i - 1, j - 1)] + 1;
                    b[start] = '\\';
                } else
                if (c[indices_to_linear(i - 1, j)] >= c[indices_to_linear(i, j - 1)])
                {
                    c[start] = c[indices_to_linear(i - 1, j)];
                    b[start] = '|';
                } else
                {
                    c[start] = c[indices_to_linear(i, j - 1)];
                    b[start] = '-';
                }
            }
            fill_tables(start + 1, end);
        }

        std::array<char, (getsize(Seq1::value())+1) * (getsize(Seq2::value())+1)> b{};
        std::array<size_t, (getsize(Seq1::value())+1) * (getsize(Seq2::value())+1)> c{};
        std::array<value_type, max_seq_size<Seq1,Seq2>()> subsequence{};

        void linear_print_lcs(value_type const * const x, int i, int j) noexcept
        {
            if ((i == 0) || (j == 0))
            {
                return;
            }
            if (b[indices_to_linear(i, j)] == '\\')
            {
                linear_print_lcs (x, i - 1, j - 1);
                subsequence[subsequence_size++] = x[i - 1];
            } else
            if (b[indices_to_linear(i, j)] == '|')
            {
                linear_print_lcs (x, i - 1, j);
            } else
            {
                linear_print_lcs (x,i, j - 1);
            }
        }

        size_type subsequence_size;

    public:
        constexpr ch15_4_subsequence(Seq1, Seq2) : subsequence_size{0}
        {
            fill_tables(0, (m+1)*(n+1));
            linear_print_lcs(Seq1::value(), m, n);
        }

        [[nodiscard]] size_type size() const
        {
            return subsequence_size;
        }

        [[nodiscard]] bool_type empty() const
        {
            return (subsequence_size == 0);
        }

/// \name Element access
/// @{
    public:
        const_reference operator [](size_type index) const
        {
            assert (index < size());
            return subsequence[index];
        }
/// @}

/// \name Iteration
/// @{
    public:
        [[nodiscard]] const_iterator begin() const
        {
            return subsequence.begin();
        }
        [[nodiscard]] const_iterator end() const
        {
            return subsequence.begin() + subsequence_size;
        }
        [[nodiscard]] const_reverse_iterator rbegin() const
        {
            return subsequence.rbegin() + (max_seq_size<Seq1, Seq2>() - subsequence_size);
        }
        [[nodiscard]] const_reverse_iterator rend() const
        {
            return subsequence.rend();
        }
        operator const_iterator() // no explicit
        {
            return subsequence.begin();
        }
    };
}

int main()
{
    CHAR_CONST_PTR(0A00B000FB00);
    _0A00B000FB00 seq1;

    CHAR_CONST_PTR(OOAOOOBFOOOBOOO);
    _OOAOOOBFOOOBOOO seq2;

    using namespace cormen;

    ch15_4_subsequence s (seq1, seq2);

    assert(s.size() == 4);
    assert(!s.empty());
    assert(s[0] == 'A');
    assert(s[1] == 'B');
    assert(s[2] == 'F');
    assert(s[3] == 'B');
    assert(strcmp(s, "ABFB") == 0);

    // operator char const *
    std::cout << s << std::endl;

    for (auto elem : s)
    {
        std::cout << elem;
    }
    std::cout << std::endl;

    auto riter = s.rbegin();
    while (riter != s.rend())
    {
        std::cout << *riter;
        riter++;
    }
    std::cout << std::endl;
    return 0;
}
