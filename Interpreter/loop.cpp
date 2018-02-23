#include "utils.h"

#include <cassert>
#include <functional>

long LoopInterpret(const std::vector<unsigned char>& program)
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
        if(program[pc] > 8)
        {
            std::cout << pc << " " << program[pc] << " " << programSize << std::endl;
            assert(false && "Unknown opcode");
            pc += 2;
        }
        else if(program[pc] == 8)
        {
            return value;
        }

        funcs[program[pc]](program, pc, value);
    }
     
    assert(false && "Program has no END");
    return value;
}

int main()
{
    ComputeMeanExecTime(LoopInterpret, GenerateProgram());
    return 0;
}