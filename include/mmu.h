#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include "pagetable.h"

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

    uint32_t createProcess(int text,int data,PageTable* page_table);
    void print();
    void listProcesses();
    void allocateMemory(int pid,std::string name,std::string datatype,int n_elements,PageTable* pagetable);
    Mmu::Variable* getVariable(int PID, std::string var_name);
    void terminate( int PID );
    void free(int PID, std::string name);
    //int getVarSize(std::string var_name);
    //int getVarStride(std::string var_name);

private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;
    
    Mmu::Process* getProcess(int pid);

};


#endif // __MMU_H_
