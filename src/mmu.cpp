#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess(int text,int data)
{

    //TODO doesn't currently create text, data, stack; just the free space

    Process *proc = new Process();

    proc->pid = _next_pid;

    Variable* var = new Variable();

    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;

    var->type = Mmu::Datatype::Free;

    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
        }
    }
}

void Mmu::listProcesses()
{// used for command 'print processes'
    std::cout << "PIDs:" << std::endl;
    
    for(auto it = _processes.begin();it != _processes.end();it++)
    {
        Process* process = *it;

        std::cout << process->pid <<std::endl;
    }
}

Mmu::Process* Mmu::getProcess(int pid)
{
    return _processes[pid-1024];
}

void Mmu::allocateMemory(int pid,std::string name,Mmu::Datatype datatype,int n_elements,PageTable* pagetable)
{
    int container_size;

    switch(datatype)
    {
        case(Mmu::Datatype::Char):
        {
            container_size = 1;
            break;
        }
        case(Mmu::Datatype::Short):
        {
            container_size = 2;
            break;
        }
        case(Mmu::Datatype::Int):
        case(Mmu::Datatype::Float):
        {
            container_size = 4;
            break;
        }
        case(Mmu::Datatype::Long):
        case(Mmu::Datatype::Double):
        {
            container_size = 8;
            break;
        }
    }

    int bytes_needed = container_size * n_elements;
}
