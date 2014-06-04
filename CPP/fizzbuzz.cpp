#include <iostream>

template<size_t N, size_t Div3, size_t Div5>
struct FizzBuzz
{
	static void print()
	{
		std::cout << N << std::endl;
		Fizzbuzz<N-1>::print();
	}
};

template<size_t N>
struct FizzBuzz<N, 0, 0>
{
	static void print()
	{
		std::cout << "FizzBuzz" << std::endl;
		Fizzbuzz<N-1>::print();
	}
};

template<size_t N, size_t Div3>
struct FizzBuzz<N, Div3, 0>
{
	static void print()
	{
		std::cout << "Buzz" << std::endl;
		Fizzbuzz<N-1>::print();
	}
};

template<size_t N, size_t Div5>
struct FizzBuzz<N, 0, Div5>
{
	static void print()
	{
		std::cout << "Fizz" << std::endl;
		Fizzbuzz<N-1>::print();
	}
};

template<>
struct FizzBuzz<0>
{
	static void print()
	{
		std::cout << 0 << std::endl;
	}
};


int main()
{
	std::cout << "Fizzbuzz for 0 to 100" << std::endl;
	RunFizzBuzz<100>; 
}