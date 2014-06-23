#include <fstream>
#include <iostream>
#include <string>
#include <string.h> // for memset

class Interpreter {
public:
  Interpreter() : m_Index(0) { memset(m_Tab, 0, 30000); }

  ~Interpreter() {}

  void Interpret(const char *in_FileName, std::string &out_Error) {
    const char *l_Program = Read(in_FileName);
    if (l_Program == nullptr) {
      out_Error = "Unable to open the program file";
      return;
    }

    // Loop until null is found
    while (*l_Program) {
      if (*l_Program == '[') {
        int l_LoopLength = ExecuteLoop(l_Program, out_Error);

        if (l_LoopLength < 0) {
          return;
        }

        while (--l_LoopLength > 0) {
          ++l_Program;
        }
      } else {
        ExecuteInstruction(*l_Program);
      }
      ++l_Program;
    }
  }

private:
  const char *Read(const char *in_FileName)
  {
        std::fstream l_Stream;
        l_Stream.open(in_FileName);

        // Failed to open
        if(!l_Stream.is_open())
        return nullptr;
        
        l_Stream.seekg(0, std::ios::end);
        int l_FileLength = l_Stream.tellg();
        l_Stream.seekg(0, std::ios::beg);
        
        // Failed to get length
        if(l_FileLength == -1)
        return nullptr;
        
        char * l_Content = new char[l_FileLength];
        l_Stream.read(l_Content, l_FileLength);
        return l_Content;
  }

  int FindLoopLength(const char *in_CurrentInstruction, std::string &out_Error)
  {
    const char *l_Tmp = in_CurrentInstruction;
    int l_Skip = 0;
    int l_LoopLength = 0;

    while (*l_Tmp) {
      l_Tmp++;

      if (*l_Tmp == ']') {
        if (!l_Skip) {
          return l_LoopLength;
        } else {
          l_Skip--;
        }
      } else if (*l_Tmp == '[') {
        l_Skip++;
      }

      l_LoopLength++;
    }

    out_Error = "Error : Missing ]";
    return -1;
  }

  int ExecuteLoop(const char *in_LoopBody, std::string &out_Error) {
    int l_LoopLength = FindLoopLength(in_LoopBody, out_Error);

    if (!out_Error.empty()) {
      return -1;
    }

    int l_Cpt = 0;

    while (true) {
      ExecuteInstruction(*++in_LoopBody);

      if (++l_Cpt >= l_LoopLength) {
        if (m_Tab[m_Index]) {
          while (--l_Cpt > 0) {
            --in_LoopBody;
          }
        } else {
          return l_LoopLength;
        }
      }
    }
  }

  void ExecuteInstruction(const char &in_Instruction) {
    switch (in_Instruction) {
    case '<': {
      if (--m_Index < 0) {
        m_Index = 29999;
      }
      break;
    }
    case '>': {
      if (++m_Index >= 30000) {
        m_Index = 0;
      }
      break;
    }
    case '+': {
      ++m_Tab[m_Index];
      break;
    }
    case '-': {
      --m_Tab[m_Index];
      break;
    }
    case '.': {
      std::cout.put(m_Tab[m_Index]);
      break;
    }
    case ',': {
      m_Tab[m_Index] = std::cin.get();
      break;
    }
    default: { break; }
    }
  }

private:
  unsigned char m_Tab[30000];
  int m_Index;
};

int main(int argc, char **argv) {
  Interpreter l_Interpreter;
  std::string l_Error;

  l_Interpreter.Interpret(*argv, l_Error);

  if (!l_Error.empty()) {
    std::cout << l_Error << std::endl;
  }

  return 0;
}
