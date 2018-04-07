#include "utils.h"

#include <cassert>

long DirectSwitchInterpret(const std::vector<unsigned char>& program)
{
    long value{};
    size_t pc{};
    const size_t programSize = program.size();

    while(pc < programSize)
    {
        switch(program[pc])
        {
            case 0:
                value += program[++pc];
                ++pc;
                break;
            case 1:
                value /= program[++pc];
                ++pc;
                break;
            case 2:
                value *= program[++pc];
                ++pc;
                break;
            case 3:
                value -= program[++pc];
                ++pc;
                break;
            case 4:
                value %= program[++pc];
                ++pc;
                break;
            case 5:
                value &= program[++pc];
                ++pc;
                break;
            case 6:
                value |= program[++pc];
                ++pc;
                break;
            case 7:
                value ^= program[++pc];
                ++pc;
                break;
            case 8:
                return value;
            default:
                std::cout << pc << " " << program[pc] << " " << programSize << std::endl;
                assert(false && "Unknown opcode");
                pc += 2;
                break;
        }
    }

    assert(false && "Program has no END");
    return value;
}

int main()
{
    ComputeMeanExecTime(DirectSwitchInterpret, GenerateProgram());
    return 0;
}