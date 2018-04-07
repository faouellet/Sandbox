#include "utils.h"

#include <cassert>

long DirectThreadedInterpret(const std::vector<unsigned char>& program)
{
    static void *funcs[] = { &&ADD, &&DIV, &&MUL, &&SUB, &&MOD, &&AND, &&OR, &&XOR, &&END };

    std::vector<const unsigned char*> bytecode;
    bytecode.reserve(program.size());
    for(int i = 0; i < program.size(); i += 2)
    {
        bytecode.push_back(static_cast<const unsigned char*>(funcs[program[i]]));
        bytecode.push_back(&program[i + 1]);
    }

    const int nbReps = 100;
    std::chrono::nanoseconds total{};
    for(int i = 0; i < nbReps; ++i)
    {
        const auto start = std::chrono::system_clock::now();
        
    long value{};
    const unsigned char** pc = bytecode.data();
    const void* nextFunc = *pc;
    goto *nextFunc;
ADD:
    value += static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
DIV:
    value /= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
MUL:
    value *= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
SUB:
    value -= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
MOD:
    value %= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
AND:
    value &= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
OR:
    value |= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
XOR:
    value ^= static_cast<int>(**++pc);
    nextFunc = *++pc;
    goto *nextFunc;
END:
    //return value;
    const auto elapsed = std::chrono::system_clock::now() - start;
    total += elapsed;
    }

    std::cout << "Mean elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(total / nbReps).count() << std::endl;
    
    return 0;
}

int main()
{
    DirectThreadedInterpret(GenerateProgram());
    return 0;
}