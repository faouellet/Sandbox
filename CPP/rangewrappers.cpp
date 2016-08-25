#include <iostream>
#include <iterator>
#include <vector>

////////// Reverse //////////
namespace Reverse
{
    template <typename T>
    struct RangeWrapper { T& range; };

    template <typename T>
    auto begin(RangeWrapper<T> wrapper) { return std::rbegin(wrapper.range); }

    template <typename T>
    auto end(RangeWrapper<T> wrapper) { return std::rend(wrapper.range); }

    template <typename T>
    RangeWrapper<T> Reverse(T&& range) { return{ range }; }
}

////////// Enumerate //////////
namespace Enumerate
{
    template <typename IteratorT, typename ValueT>
    class EnumerateIterator : std::iterator<std::bidirectional_iterator_tag, ValueT>
    {
    public:
        using NumberValPair = std::pair<size_t, ValueT>;

    public:
        explicit EnumerateIterator(IteratorT&& iterator) : mCount{ 0 }, mItr{ iterator } { }
        EnumerateIterator(IteratorT&& iterator, size_t startingCount) : mCount{ startingCount }, mItr{ iterator } { }

        EnumerateIterator& operator++()
        {
            ++mItr;
            ++mCount;
            return *this;
        }

        EnumerateIterator operator++(int)
        {
            auto temp{ *this };
            operator++();
            return temp;
        }

        EnumerateIterator& operator--()
        {
            --mItr;
            --mCount;
            return *this;
        }

        EnumerateIterator operator--(int)
        {
            auto temp{ *this };
            operator--();
            return temp;
        }

        bool operator==(const EnumerateIterator& enumItr) const
        {
            return (mCount == enumItr.mCount) && (mItr == enumItr.mItr);
        }

        bool operator!=(const EnumerateIterator& enumItr) const
        {
            return !(*this == enumItr);
        }

        NumberValPair operator*()
        {
            return std::make_pair(mCount, *mItr);
        }

        NumberValPair operator*() const
        {
            return std::make_pair(mCount, *mItr);
        }

    private:
        size_t mCount;
        IteratorT mItr;
    };

    template <typename T>
    struct RangeWrapper { T& range; };

    template <typename T>
    auto begin(RangeWrapper<T> wrapper) { return EnumerateIterator<decltype(std::begin(wrapper.range)), std::iterator_traits<decltype(std::begin(wrapper.range))>::value_type>(std::begin(wrapper.range)); }

    template <typename T>
    auto end(RangeWrapper<T> wrapper) { return EnumerateIterator<decltype(std::end(wrapper.range)), std::iterator_traits<decltype(std::end(wrapper.range))>::value_type>(std::end(wrapper.range), std::distance(std::begin(wrapper.range), std::end(wrapper.range))); }

    template <typename T>
    RangeWrapper<T> Enumerate(T&& range) { return{ range }; }
}


int main()
{
    int arr[] = { 1,2,3,4,5 };
    std::vector<int> vec{ 1,2,3,4,5 };
    
    for (const auto& val : arr)
        std::cout << val;
    std::cout << "\n";

    for (const auto& val : Reverse::Reverse(arr))
        std::cout << val;
    std::cout << "\n";

    for (const auto& nbVal : Enumerate::Enumerate(vec))
        std::cout << nbVal.first;
    std::cout << "\n";
}