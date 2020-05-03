#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess(int text,int data,PageTable* page_table)
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

    int pages_needed = (int) v_addr / page_table->pageSize();

    if(v_addr % page_table->pageSize() != 0)
        ++pages_needed;

    for(int i = 0;i < pages_needed;i++) //NOT SUPER WELL TESTED
    {
        page_table->addEntry(proc->pid,i);
    }
    
    // for(int i = 0;i < proc->variables.size();i++)
    // {
    //     std::cout << "Name: " << proc->variables[i]->name << " | Address: " << proc->variables[i]->virtual_address << " | Size: " << proc->variables[i]->size << std::endl;
    // }

    _next_pid++;
    return proc->pid;
}

// IMPLEMENTED BY JESSE, MIGHT BE FUCKED
    // lol
Mmu::Variable* Mmu::getVariable(int PID, std::string var_name){

    int i, j;
    // bool found = false;
    for (i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == PID){

            for (j = 0; j < _processes[i]->variables.size(); j++)
            {
                if(_processes[i]->variables[j]->name == var_name){
                    return _processes[i]->variables[j];
                    break;
                }
            }
        }        
    }
    return NULL; 
}


/*


// IMPLEMENTED BY JESSE, MIGHT BE FUCKED
int Mmu::getVarSize(std::string var_name){

    int i;
    bool found = false;
    for(i=0; i<proc->variables.size(); i++){

        if(proc->variables[i] == var_name){
            found = true;
            break;
        }
    }

    if(found){

        return proc->variables[i]->size;
    }
    else{
        return -1;
    }
}

// IMPLEMENTED BY JESSE, MIGHT BE FUCKED
int Mmu::getVarStride(std::string var_name){

    int i;
    bool found = false;
    for(i=0; i<proc->variables.size(); i++){

        if(proc->variables[i] == var_name){
            found = true;
            break;
        }
    }

    if(found){

        int stride = -1;

        switch(proc->variables[i]->type)    // gets size of variable type in bytes
        {
            case(Mmu::Datatype::Char):
            {
                stride = 1;
                break;
            }
            case(Mmu::Datatype::Short):
            {
                stride = 2;
                break;
            }
            case(Mmu::Datatype::Int):
            case(Mmu::Datatype::Float):
            {
                stride = 4;
                break;
            }
            case(Mmu::Datatype::Long):
            case(Mmu::Datatype::Double):
            {
                stride = 8;
                break;
            }
        }
    }
    
    return stride;
}


*/

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            std::cout << ' ' << _processes[i]->pid << " |";
            std::cout << ' ' << _processes[i]->variables[j]->name;
            for(int k=_processes[i]->variables[j]->name.length(); k<14; k++) std::cout << ' ';
            std::cout << "|   ";
            printf("0x%08X |", _processes[i]->variables[j]->virtual_address);
            for(int k=std::to_string(_processes[i]->variables[j]->size).length(); k<11; k++) std::cout << ' ';
            std::cout << _processes[i]->variables[j]->size << std::endl;
        }
    }
}

void Mmu::listProcesses()
{// used for command 'print processes'
    //std::cout << "PIDs:" << std::endl;
    
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

bool variableSort(Mmu::Variable* x,Mmu::Variable* y)
{
    return x->virtual_address < y->virtual_address;
}

void Mmu::allocateMemory(int pid, std::string name, Mmu::Datatype datatype, int n_elements, PageTable* pagetable){

    Mmu::Process* proc = getProcess(pid);

    //make sure its an existing process
    if(proc == NULL)
    {
        std::cout << "ERROR: process \"" << pid << "\" does not exist" << std::endl;
        return;
    }

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

    Variable* var = new Variable();
    var->name = name;
    var->size = bytes_needed;

    for(int i=0; i<proc->variables.size(); i++){

        if(proc->variables[i]->name == "<FREE_SPACE>" ){

            //set base addr to beginning of <FREE_SPACE>
            var->virtual_address = proc->variables[i]->virtual_address;
            //update <FREE_SPACE> stats
            proc->variables[i]->virtual_address += (bytes_needed + 1);
            proc->variables[i]->size -= bytes_needed;
        }
    } 
    proc->variables.push_back(var);

    std::sort(proc->variables.begin(),proc->variables.end(),variableSort);

    // calculate number of new pages needed

    int pages_needed = 0;

    // space left in the current page
    int space_remaining = pagetable->pageSize() - (var->virtual_address % pagetable->pageSize());  //DOESN'T WORK

    // std::cout << "space remaining: " << space_remaining << std::endl;

    if(var->size > space_remaining) // if the new variable can't fit in the last page, add new pages
        pages_needed = (pages_needed/pagetable->pageSize());

    for(int i = 0;i < pages_needed;i++) //NOT SUPER WELL TESTED
    {
        pagetable->addEntry(proc->pid,i);
    }
}
