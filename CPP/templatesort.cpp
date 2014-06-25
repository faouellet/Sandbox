#include <iostream>
#include <tuple>
#include <type_traits>

template<int... Tail> struct StaticIntList { }; 

template<class T> struct Printer;

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

int main()
{
    using SIL = StaticIntList<4,3,2,1>;
    Printer<SIL>::print();
}
