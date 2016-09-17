#include <functional>
#include <future>
#include <iostream>
#include <vector>

template <typename FuncT>
class Task
{
public:
    explicit Task(FuncT&& func) : mFunc{ std::forward<FuncT>(func) } { }

    template <typename ...ArgsT>
    auto operator()(ArgsT&&... args) const
    {
        return std::bind(mFunc, std::placeholders::_1, std::forward<ArgsT>(args)...);
    }

private:
    FuncT mFunc;
};

template <typename FuncT>
Task<FuncT> CreateTask(FuncT&& func)
{
    return Task<FuncT>{ std::forward<FuncT>(func) };
}

template<typename T, typename FuncT>
std::vector<std::future<std::result_of_t<FuncT()>>> operator|(const std::vector<std::future<void>>& args, const FuncT& func)
{
    return{ std::async(std::launch::async, func) };
}

template<typename T, typename FuncT>
std::vector<std::future<std::result_of_t<FuncT(T)>>> operator|(const std::vector<std::future<std::result_of_t<FuncT(T)>>>& args, const FuncT& func)
{
    std::vector<std::future<std::result_of_t<FuncT(T)>>> res;

    for (const auto& arg : args)
    {
        res.emplace_back(std::async(std::launch::async, func, arg));
    }

    return res;
}

template<typename T, typename FuncT>
std::vector<std::future<std::result_of_t<FuncT(T)>>> operator|(const std::vector<T>& args, const FuncT& func)
{
    std::vector<std::future<std::result_of_t<FuncT(T)>>> res;

    for (const auto& arg : args)
    {
        res.emplace_back(std::async(std::launch::async, func, arg));
    }

    return res;
}

int main()
{
    std::vector<int> input{ 1,2,3,4,5,6 };
    
    auto add = CreateTask([](int x, int y) { return x + y; });
    auto mul = CreateTask([](int x, int y) { return x * y; });

    auto y = input | add(2) | mul(5);
}
