#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess(int text,int data, PageTable* page_table)
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

// IMPLEMENTED BY JESSE, MIGHT BE FUCKED
Mmu::Variable* Mmu::getVariable(int PID, std::string var_name){

    int i, j;
    bool found = false;
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
            if(_processes[i]->variables[j]->name != "<FREE_SPACE>" ){
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


void Mmu::allocateMemory(int pid, std::string name, std::string datatype, int n_elements, PageTable* pagetable){

    Mmu::Process* proc = getProcess(pid);
    Variable* var = new Variable();
    int page_size = pagetable->pageSize();

    int container_size;

    //make sure its an existing process
    if( Mmu::getProcess(pid) != NULL ){
        if(datatype == "char") container_size = 1;
        else if(datatype == "short") container_size = 2;
        else if(datatype == "int" || "float") container_size = 4;
        else if(datatype == "long" || "double") container_size = 8;
    }

    int bytes_needed = container_size * n_elements;
    var->name = name;
    var->size = bytes_needed;
    var->type = datatype;
    int i;

    for(i=0; i<proc->variables.size(); i++){

        //if a free space is found
        if( proc->variables[i]->name == "<FREE_SPACE>" ){

            //if it is large enough to fit the new variable
            if( proc->variables[i]->size >= var->size){

                //set base addr to beginning of <FREE_SPACE>
                var->virtual_address = proc->variables[i]->virtual_address;
                //update <FREE_SPACE> stats
                proc->variables[i]->virtual_address += bytes_needed + 1;
                proc->variables[i]->size -= bytes_needed;
                //if the free space is completely filled
                if(proc->variables[i]->size <= 0){
                    proc->variables.erase(proc->variables.begin() + (i-1));
                }
            }
        }
    } 
    proc->variables.push_back(var);

    pagetable->addEntry( pid, proc->variables[i]->virtual_address/page_size );
    pagetable->addEntry( pid, var->virtual_address/page_size );
    

    // std::cout << "VARIABLES BEFORE SORT" << std::endl;

    // for(auto it = proc->variables.begin();it != proc->variables.end();it++)
    // {
    //     Variable* test = *it;

    //     std::cout << test->name << std::endl;
    // }

   /* std::sort(proc->variables.begin(),proc->variables.end(),variableSort);

    // std::cout << "VARIABLES AFTER SORT" << std::endl;

    // for(auto it = proc->variables.begin();it != proc->variables.end();it++)
    // {
    //     Variable* test = *it;

    //     std::cout << test->name << std::endl;
    // }    

    // calculate number of new pages needed

    int pages_needed = 0;

    int space_remaining = pagetable->pageSize() - (var->virtual_address % pagetable->pageSize());  //DOESN'T WORK

    std::cout << "space remaining: " << space_remaining << std::endl;
        // space left in the current page

    if(var->size > space_remaining) // if the new variable can't fit in the last page, add new pages
        pages_needed = (pages_needed/pagetable->pageSize());

    for(int i = 0;i < pages_needed;i++) //NOT SUPER WELL TESTED
    {
        pagetable->addEntry(proc->pid,i);
    }
    */
}
