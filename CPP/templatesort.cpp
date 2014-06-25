#include <iostream>
#include <tuple>
#include <type_traits>

/* template<class Op>
struct filter
{
	using type = std::tuple<>;
};

template<class Op, int Head, int... Tail>
struct filter
{
	using type = std::conditional<Op<Head>::value, 
				 merge<Head, typename filter<Tail...>::type>,
				 typename filter<Tail...>::type
				 >::type;
};

template<typename... Args>
struct merge
{
	using type = std::tuple<Args...>;
}; */

template<int I>
struct IsNotNegative
{
	using value = I > 0;
};

template<class Tuple, size_t Pos>
std::enable_if<IsNotNegative<Pos>::value, std::ostream&>
Print(std::ostream& os, const Tuple& t)
{

}

template<class... Args>
std::ostream& operator<<(ostream& os, const std::tuple<Args...>& tuple)
{
	Print<decltype<tuple>, std::tuple_size<decltype<tuple>>>(os, tuple);
}

int main()
{
	std::tuple<1,2,3> Tuple;
	
	std::cout << Tuple << std::endl;
}