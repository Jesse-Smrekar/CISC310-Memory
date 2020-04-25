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
    Process *proc = new Process();
    std::cout << "Process created\n";

    proc->pid = _next_pid;
    std::cout << "Pid set to " << proc->pid << std::endl;

    Variable* var = new Variable();
    std::cout << "Variable created" << std::endl;

    var->name = "<FREE_SPACE>";
    std::cout << "free space named" << std::endl;

    var->virtual_address = 0;
    std::cout << "address set" << std::endl;

    var->size = _max_size;
    std::cout << "maxsize set" << std::endl;

    var->type = Mmu::Datatype::Free;
    std::cout << "datatype set, about to add variable to vector size " << proc->variables.size() << std::endl;

    proc->variables.push_back(var);
    std::cout << "variable added, size is " << proc->variables.size() << std::endl;

    std::cout << "size = " << _processes.size() << std::endl;    // seg fault here also

    if(_processes.size() > 0)
        std::cout << _processes[0]->pid;    // seg fault occurs here, but not on size call

    std::cout << "incoming error" << std::endl;
    _processes.push_back(proc); //TODO trying to push a second process throws a seg fault

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
