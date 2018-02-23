#include "utils.h"

#include <cassert>

long IndirectThreadedInterpret(const std::vector<unsigned char>& program)
{
    static void* funcs[] = { &&ADD, &&DIV, &&MUL, &&SUB, &&MOD, &&AND, &&OR, &&XOR, &&END };
    long value{};
    int pc{};
    goto *funcs[program[pc]];
ADD:
    value += static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
DIV:
    value /= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
MUL:
    value *= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
SUB:
    value -= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
MOD:
    value %= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
AND:
    value &= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
OR:
    value |= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
XOR:
    value ^= static_cast<int>(program[++pc]);
    goto *funcs[program[++pc]];
END:
    return value;
}

int main()
{
    ComputeMeanExecTime(IndirectThreadedInterpret, GenerateProgram());
    return 0;
}