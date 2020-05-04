#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include "pagetable.h"

class Mmu {
public:
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
        int page_count;
    } Process;
    
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess(int text,int data,PageTable* page_table);
    void print();
    void listProcesses();
    void allocateMemory(int pid,std::string name,std::string datatype,int n_elements,PageTable* pagetable);
    Mmu::Variable* getVariable(int PID, std::string var_name);
    Mmu::Process* getProcess(int pid);
    void terminate( int PID );
    void free(int PID, std::string NAME,PageTable* pagetable);

private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;
    void condenseFrees(Mmu::Process* proc);
    void freeEmpties(Mmu::Process* proc,int page_size,PageTable* pagetable);
};


#endif // __MMU_H_
