#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

void Mmu::free(int PID, std::string NAME,PageTable* pagetable){

    Mmu::Process *proc = getProcess(PID);

    for(int i=0; i<proc->variables.size(); i++){

        if(proc->variables[i]->name == NAME){

            Variable* freed = proc->variables[i];

            freed->name = "FREE_SPACE";
            freed->type = "free";
            break;
        }
    }

    condenseFrees(proc);
    freeEmpties(proc,pagetable->pageSize(),pagetable);
}


void Mmu::terminate( int PID ){

    for(int i=0;i < _processes.size();i++)
    {
        if(_processes[i]->pid == PID){
            _processes.erase(_processes.begin() + i);
            break;
        }
    }
}

uint32_t Mmu::createProcess(int text,int data,PageTable* page_table)
{
    Process *proc = new Process();
    proc->pid = _next_pid;
    proc->page_count = 0;

    int v_addr = 0;

    Variable* text_var = new Variable();    // initialize text variable

    text_var->name = "<TEXT>";
    text_var->virtual_address = v_addr;
    text_var->size = text;
    text_var->type = "text";

    proc->variables.push_back(text_var);
    v_addr += text;

    if(data > 0)
    {
        Variable* data_var = new Variable();    // initialize data variable

        data_var->name = "<GLOBALS>";
        data_var->virtual_address = v_addr;
        data_var->size = data;
        data_var->type = "data";

        proc->variables.push_back(data_var);
        v_addr += data;
    }

    Variable* stack_var = new Variable();   // initialize stack variable

    stack_var->name = "<STACK>";
    stack_var->virtual_address = v_addr;
    stack_var->size = 65536;
    stack_var->type = "stack";

    proc->variables.push_back(stack_var);
    v_addr += 65536;

    Variable* var = new Variable();

    var->name = "<FREE_SPACE>";
    var->virtual_address = v_addr;
    var->size = _max_size - text - data - 65536;

    var->type = "free";

    proc->variables.push_back(var);

    _processes.push_back(proc);

    int pages_needed = (int) v_addr / page_table->pageSize();

    if(v_addr % page_table->pageSize() != 0)
        ++pages_needed;

    for(int i = 0;i < pages_needed;i++)
    {
        page_table->addEntry(proc->pid,proc->page_count++);
    }

    _next_pid++;
    return proc->pid;
}

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

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            if(_processes[i]->variables[j]->type != "free") // avoids printing free spaces
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
}

void Mmu::listProcesses()
{// used for command 'print processes'
    
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

void Mmu::allocateMemory(int pid, std::string name, std::string datatype, int n_elements, PageTable* pagetable){

    Mmu::Process* proc = getProcess(pid);

    //make sure its an existing process
    if(proc == NULL)
    {
        std::cout << "ERROR: process \"" << pid << "\" does not exist" << std::endl;
        return;
    }

    Variable* var = new Variable();

    int container_size;

    if(datatype == "char"){
        container_size = 1;
    }

    else if(datatype == "short"){
        container_size = 2;
    }

    else if(datatype == "int" || datatype == "float"){
        container_size = 4;
    }

    else if(datatype == "double" || datatype == "long"){
        container_size = 8;
    }

    int bytes_needed = container_size * n_elements;

    if(bytes_needed < 0)
    {
        std::cout << "ERROR: memory overflow, aborting allocation" << std::endl;
        return;
    }

    var->name = name;
    var->size = bytes_needed;
    var->type = datatype;

    //iterate through list
    //if variable is a free space large enough:
        // place new variable at beginning
        // resize free space variable to fit

    for(auto it = proc->variables.begin();it != proc->variables.end();++it)
    {
        Variable* ptr = *it;

        if(ptr->type == "free" && ptr->size >= var->size)   // var is a large enough empty space
        {
            var->virtual_address = ptr->virtual_address;

            ptr->virtual_address += var->size;
            ptr->size -= var->size;

            break;
        }
    }

    proc->variables.push_back(var);

    std::sort(proc->variables.begin(),proc->variables.end(),variableSort);

    int new_page_count = proc->variables.back()->virtual_address / pagetable->pageSize();

    if(proc->variables.back()->virtual_address % pagetable->pageSize() != 0)
        new_page_count++;

    if(new_page_count > proc->page_count)
        for(int i = 0;i < new_page_count;i++)
            pagetable->addEntry(proc->pid,i);

    freeEmpties(proc,pagetable->pageSize(),pagetable);

    proc->page_count = new_page_count;

    std::cout << var->virtual_address << std::endl;
}

void Mmu::condenseFrees(Mmu::Process* proc)
{// combines free spaces together into larger free spaces

    bool done = false;

    while(!done)
    {
        done = true;

        for(int i = 0;(i+1) < proc->variables.size();i++)
        {
            if (proc->variables[i]->type == "free" && proc->variables[i+1]->type == "free") // two frees together
            {
                done = false;

                proc->variables[i]->size += proc->variables[i+1]->size;

                proc->variables.erase(proc->variables.begin()+i+1);
                break;
            }
        }
    }
}

void Mmu::freeEmpties(Mmu::Process* proc,int page_size,PageTable* pagetable)
{
    bool* used = new bool[proc->page_count];

    for(int i = 0;i < proc->page_count;i++)
        used[i] = false;

    for(auto it = proc->variables.begin();it != proc->variables.end();it++)
    {
        if((*it)->type != "free")
        {
            for(int i = 0;i < ((*it)->size);i++)
            {
                int byte = (*it)->virtual_address + i;
                used[byte / page_size] = true;
            }
        }
    }

    for(int i = 0;i < proc->page_count;i++)
    {
        if(used[i] == false)
        {
            pagetable->free(proc->pid,i);
        }
    }
}