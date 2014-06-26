/*
 * Implementation of a quicksort for integers using template metaprogramming
 * */

#include <iostream>
#include <tuple>
#include <type_traits>

// Compile-time representation of a list of integers
template<int... Vals> struct StaticIntList { };

////////////////////////////////////////////////////////////////////////////////
// Printing
template<class> struct Printer;

template<int Head, int... Tail>
struct Printer<StaticIntList<Head, Tail...>>
{
    static void print()
    {
        std::cout << Head << std::endl;
        Printer<StaticIntList<Tail...>>::print();
    }
};

template<>
struct Printer<StaticIntList<>>
{
    static void print() { }
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prepend
template<int, class> struct Prepend;

template<int Val, int... Vals>
struct Prepend<Val, StaticIntList<Vals...>>
{
    using type = StaticIntList<Val, Vals...>;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Concatenation
template<class, class> struct Concat;

template<int Head, int... Tail1, int... Tail2>
struct Concat<StaticIntList<Head, Tail1...>, StaticIntList<Tail2...>>
{
    using type = typename Prepend<Head, 
        typename Concat<StaticIntList<Tail1...>, StaticIntList<Tail2...>>::type
        >::type; 
};

template<int... Tail>
struct Concat<StaticIntList<>, StaticIntList<Tail...>>
{
    using type = StaticIntList<Tail...>; 
};
////////////////////////////////////////////////////////////////////////////////

int main()
{
    using SIL1 = StaticIntList<4,3,2,1>;
    using SIL2 = StaticIntList<4,3,2,1>;
    Printer<typename Concat<SIL1, SIL2>::type>::print();
}
