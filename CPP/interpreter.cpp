#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

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

    std::chrono::nanoseconds total{};
    for(int i = 0; i < 100; ++i)
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

    std::cout << "Mean elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(total / 30).count() << std::endl;
    
    return 0;
}

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

long DirectSwitchInterpret(const std::vector<unsigned char>& program)
{
    long value{};
    int pc{};
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

std::vector<unsigned char> GenerateProgram()
{
    std::vector<unsigned char> program;

    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<> opcodeDistribution(0,7);
    std::uniform_int_distribution<> operandDistribution(1,100);

    for(unsigned long iOp = 0; iOp < 1000000; ++iOp)
    {
        program.emplace_back(opcodeDistribution(engine));
        program.emplace_back(operandDistribution(engine));
    }

    program.emplace_back(8);   // END
    program.emplace_back(0);   // Dummy operand
    
    return program;    
}

int main()
{
    std::vector<unsigned char> program{ GenerateProgram() };

    auto TimedExec = [&program](auto&& callable)
    {
        std::chrono::nanoseconds total{};
        //long result{};
        
        for(int i = 0; i < 100; ++i)
        {
            const auto start = std::chrono::system_clock::now();
            /*result = */callable(program);
            const auto elapsed = std::chrono::system_clock::now() - start;
            total += elapsed;
        }

        //std::cout << "Result: " << result << std::endl;
        std::cout << "Mean elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(total / 30).count() << std::endl;
    };

    TimedExec(LoopInterpret);
    TimedExec(IndirectSwitchInterpret);
    TimedExec(DirectSwitchInterpret);
    TimedExec(IndirectThreadedInterpret);
    //TimedExec(DirectThreadedInterpret);
    DirectThreadedInterpret(program);

    return 0;
}
