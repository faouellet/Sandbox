#include "utils.h"

#include <cassert>
#include <functional>

long IndirectSwitchInterpret(const std::vector<unsigned char>& program)
{
    static std::function<void(const std::vector<unsigned char>&, int&, long&)> funcs[] = {
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value += program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value /= program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value *= program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value -= program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value %= program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value &= program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value |= program[++pc]; ++pc; },
        [](const std::vector<unsigned char>& program, int& pc, long& value){ value ^= program[++pc]; ++pc; }
    };

    long value{};
    int pc{};
    const size_t programSize = program.size();

    while(pc < programSize)
    {
        switch(program[pc])
        {
            case 0:
                funcs[program[pc]](program, pc, value);
                break;
            case 1:
                funcs[program[pc]](program, pc, value);
                break;
            case 2:
                funcs[program[pc]](program, pc, value);
                break;
            case 3:
                funcs[program[pc]](program, pc, value);
                break;
            case 4:
                funcs[program[pc]](program, pc, value);
                break;
            case 5:
                funcs[program[pc]](program, pc, value);
                break;
            case 6:
                funcs[program[pc]](program, pc, value);
                break;
            case 7:
                funcs[program[pc]](program, pc, value);
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
    ComputeMeanExecTime(IndirectSwitchInterpret, GenerateProgram());
    return 0;
}