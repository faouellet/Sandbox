#include <iostream>
#include <vector>

int interpret(const std::vector<unsigned char> & bytecode)
{
    static void *funcs[] = { &&ADD, &&DIV, &&MUL, &&SUB, &&END };
    int value = 0;
    int pc = 0;
    goto *funcs[pc];
ADD:
    value += static_cast<int>(bytecode[++pc]);
    goto *funcs[bytecode[++pc]];    // Beurk
DIV:
    value += static_cast<int>(bytecode[++pc]);
    goto *funcs[bytecode[++pc]];    // Yark
MUL:
    value += static_cast<int>(bytecode[++pc]);
    goto *funcs[bytecode[++pc]];    // Ewww
SUB:
    value += static_cast<int>(bytecode[++pc]);
    goto *funcs[bytecode[++pc]];    // ...
END:
    return value;
}

void insert_instruction(std::vector<unsigned char> & buffer, 
        const unsigned char opcode, const unsigned char operand)
{
    buffer.push_back(opcode);
    buffer.push_back(operand);
}

int main()
{
    std::vector<unsigned char> bytecode;

    insert_instruction(bytecode, 0, 8);  // ADD 8
    insert_instruction(bytecode, 1, 4);  // DIV 4
    insert_instruction(bytecode, 2, 16); // MUL 16
    insert_instruction(bytecode, 3, 2);  // SUB 2
    insert_instruction(bytecode, 4, 0);  // END -> Result = 30

    std::cout << "Result = " << interpret(bytecode) << std::endl;

    return 0;
}
