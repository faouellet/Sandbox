/*
 * Implementation of a quicksort for integers using template metaprogramming
 * */

#include <iostream>
#include <type_traits>

// Compile-time representation of a list of integers
template <int... Vals> struct StaticIntList {};

////////////////////////////////////////////////////////////////////////////////
// Printing
template <class> struct Printer;

template <int Head, int... Tail> struct Printer<StaticIntList<Head, Tail...>> {
  static void print() {
    std::cout << Head << " ";
    Printer<StaticIntList<Tail...>>::print();
  }
};

template <> struct Printer<StaticIntList<>> {
  static void print() { std::cout << std::endl; }
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prepend
template <int, class> struct Prepend;

template <int Val, int... Vals> struct Prepend<Val, StaticIntList<Vals...>> {
  using type = StaticIntList<Val, Vals...>;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Concatenation
template <class, class> struct Concat;

template <int Head, int... Tail1, int... Tail2>
struct Concat<StaticIntList<Head, Tail1...>, StaticIntList<Tail2...>> {
  using type = typename Prepend<
      Head, typename Concat<StaticIntList<Tail1...>,
                            StaticIntList<Tail2...>>::type>::type;
};

template <int... Tail> struct Concat<StaticIntList<>, StaticIntList<Tail...>> {
  using type = StaticIntList<Tail...>;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Filter
template <template <int, int> class, int, class> struct Filter;

template <template <int, int> class Op, int Arg, int Head, int... Tail>
struct Filter<Op, Arg, StaticIntList<Head, Tail...>> {
  using type = typename std::conditional<
      Op<Head, Arg>::value,
      typename Prepend<
          Head, typename Filter<Op, Arg, StaticIntList<Tail...>>::type>::type,
      typename Filter<Op, Arg, StaticIntList<Tail...>>::type>::type;
};

template <template <int, int> class Op, int Arg, int... Vals>
struct Filter<Op, Arg, StaticIntList<Vals...>> {
  using type = StaticIntList<Vals...>;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Less Than
template <int LHS, int RHS> struct LessThan {
  static const bool value = LHS < RHS;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Greater Equal
template <int LHS, int RHS> struct GreaterEqual {
  static const bool value = LHS >= RHS;
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Sort
template <class> struct Sort;

template <int Head, int... Tail> struct Sort<StaticIntList<Head, Tail...>> {
  using type = typename Concat<
      typename Sort<
          typename Filter<LessThan, Head, StaticIntList<Tail...>>::type>::type,
      typename Prepend<Head, typename Sort<typename Filter<
                                 GreaterEqual, Head, StaticIntList<Tail...>>::
                                               type>::type>::type>::type;
};

template <int... Vals> struct Sort<StaticIntList<Vals...>> {
  using type = StaticIntList<Vals...>;
};
////////////////////////////////////////////////////////////////////////////////

int main() {
  using SIL = StaticIntList<5,3,6,2,9,3,6,4,7,5,8>;

  std::cout << "Original:" << std::endl;
  Printer<SIL>::print();

  std::cout << "Sort:" << std::endl;
  Printer<Sort<SIL>::type>::print();
}
