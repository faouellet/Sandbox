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

////////////////////////////////////////////////////////////////////////////////
// Filter
template<template<int> class, class> struct Filter;

template<template<int> class Op, int Head, int... Tail>
struct Filter<Op, StaticIntList<Head, Tail...>>
{
   using type = typename std::conditional<
         Op<Head>::value,
         typename Prepend<Head, typename Filter<Op, StaticIntList<Tail...>>::type>::type,
         typename Filter<Op, StaticIntList<Tail...>>::type
         >::type;
};

template<template<int> class Op, int... Vals>
struct Filter<Op, StaticIntList<Vals...>>
{
    using type = StaticIntList<>;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Less Than
template<int RHS> 
struct LessThan
{
    template<int LHS>
    struct Apply
    {
        static const bool value = LHS < RHS;
    };
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Sort
template<class> struct Sort;
////////////////////////////////////////////////////////////////////////////////

int main()
{
    using SIL = StaticIntList<4,3,2,1>;
    Printer<Filter<LessThan<2>::Apply, SIL>::type>::print();
}
