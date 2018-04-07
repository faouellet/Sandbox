#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using Program = std::vector<unsigned char>;

Program GenerateProgram()
{
    Program program;

    // Hardcoded seed to generate the same program every time.
    // Quite useful for benchmarking
    std::mt19937 engine(12345);

    std::uniform_int_distribution<> opcodeDistribution(0,7);
    std::uniform_int_distribution<> operandDistribution(1,100);

    for(unsigned long iOp = 0; iOp < 10000000; ++iOp)
    {
        program.emplace_back(opcodeDistribution(engine));
        program.emplace_back(operandDistribution(engine));
    }

    program.emplace_back(8);   // END
    program.emplace_back(0);   // Dummy operand
    
    return program;    
}

template <typename TFunc>
void ComputeMeanExecTime(TFunc&& func, const Program& program)
{
    std::chrono::nanoseconds total{};
    const int nbReps = 100;
    //long result{};
    
    for(int i = 0; i < nbReps; ++i)
    {
        const auto start = std::chrono::system_clock::now();
        /*result = */func(program);
        const auto elapsed = std::chrono::system_clock::now() - start;
        total += elapsed;
    }

    //std::cout << "Result: " << result << std::endl;
    std::cout << "Mean elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(total / nbReps).count() << std::endl;
}

#endif // UTILS_H