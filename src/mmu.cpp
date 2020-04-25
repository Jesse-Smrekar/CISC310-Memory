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
    proc->pid = _next_pid;

    int v_addr = 0;

    Variable* text_var = new Variable();    // initialize text variable

    text_var->name = "<TEXT>";
    text_var->virtual_address = v_addr;
    text_var->size = text;
    text_var->type = Mmu::Datatype::Text;

    proc->variables.push_back(text_var);
    v_addr += text;

    if(data > 0)
    {
        Variable* data_var = new Variable();    // initialize data variable

        data_var->name = "<GLOBALS>";
        data_var->virtual_address = v_addr;
        data_var->size = data;
        data_var->type = Mmu::Datatype::Data;

        proc->variables.push_back(data_var);
        v_addr += data;
    }

    Variable* stack_var = new Variable();   // initialize stack variable

    stack_var->name = "<STACK>";
    stack_var->virtual_address = v_addr;
    stack_var->size = 65536;
    stack_var->type = Mmu::Datatype::Stack;

    proc->variables.push_back(stack_var);
    v_addr += 65536;

    Variable* var = new Variable();

    var->name = "<FREE_SPACE>";
    var->virtual_address = v_addr;
    var->size = _max_size - text - data - 65536;

    var->type = Mmu::Datatype::Free;

    proc->variables.push_back(var);

    _processes.push_back(proc);

    //TODO doesn't interface with page table at all yet

    // for(int i = 0;i < proc->variables.size();i++)
    // {
    //     std::cout << "Name: " << proc->variables[i]->name << " | Address: " << proc->variables[i]->virtual_address << " | Size: " << proc->variables[i]->size << std::endl;
    // }

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
    for(int i = 0;i < _processes.size();i++)
    {
        if(_processes[i]->pid == pid)
            return _processes[i];
    }

    return NULL;    // input pid doesn't match any current processes
}

void Mmu::allocateMemory(int pid,std::string name,Mmu::Datatype datatype,int n_elements,PageTable* pagetable)
{
    int container_size;

    switch(datatype)    // gets size of variable type in bytes
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

    //TODO error check whether process actually exists?
}
