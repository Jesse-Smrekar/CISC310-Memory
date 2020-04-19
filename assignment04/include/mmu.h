#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

enum Datatype : uint8_t {Char,Short,Int,Long,Double,Float,Empty};

typedef struct Variable
{
    std::string name;
    int virtual_address;
    int size;
    Datatype type;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;

public:
    enum Datatype : uint8_t {Char,Short,Int,Long,Double,Float};

    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void print();

    void listProcesses();
};

#endif // __MMU_H_
