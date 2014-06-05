#include <iostream>

template<size_t N, size_t Div3, size_t Div5>
struct FizzBuzz
{
	static void print()
	{
		std::cout << N << std::endl;
		FizzBuzz<N-1, (N-1)%3, (N-1)%5>::print();
	}
};

template<size_t N>
struct FizzBuzz<N, 0, 0>
{
	static void print()
	{
		std::cout << "FizzBuzz" << std::endl;
		FizzBuzz<N-1, (N-1)%3, (N-1)%5>::print();
	}
};

template<size_t N, size_t Div3>
struct FizzBuzz<N, Div3, 0>
{
	static void print()
	{
		std::cout << "Buzz" << std::endl;
		FizzBuzz<N-1, (N-1)%3, (N-1)%5>::print();
	}
};

template<size_t N, size_t Div5>
struct FizzBuzz<N, 0, Div5>
{
	static void print()
	{
		std::cout << "Fizz" << std::endl;
		FizzBuzz<N-1, (N-1)%3, (N-1)%5>::print();
	}
};

template<size_t Div3, size_t Div5>
struct FizzBuzz<0, Div3, Div5>
{
	static void print()
	{
		std::cout << 0 << std::endl;
	}
};

template<>
struct FizzBuzz<0, 0, 0>
{
	static void print()
	{
		std::cout << 0 << std::endl;
	}
};

template<size_t N>
struct RunFizzBuzz
{
    static void print()
    {
        FizzBuzz<N, N%3, N%5>::print();
    }
};

int main()
{
	std::cout << "Fizzbuzz for 100" << std::endl;
	RunFizzBuzz<100>::print(); 
}
