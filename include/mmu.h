#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

class Mmu {
public:
    enum Datatype : uint8_t {Char,Short,Int,Long,Double,Float,Free,Text,Data,Stack};

    typedef struct Variable
    {
        std::string name;
        int virtual_address;
        int size;
        std::string type;
    } Variable;

    typedef struct Process
    {
        uint32_t pid;
        std::vector<Variable*> variables;
    } Process;
    
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess(int text,int data);
    void print();

    void listProcesses();

private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;

};


#endif // __MMU_H_
