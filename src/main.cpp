#include <iostream>
#include <string>
#include <bits/stdc++.h> 
#include "mmu.h"
#include "pagetable.h"

typedef struct Hardware
{
    PageTable* page_table;
    Mmu* mmu;
    uint8_t* memory;
} Hardware;

void printStartMessage(int page_size);
void tokenize(std::string const &str,const char delim,std::vector<std::string> &out);
void create(std::vector<std::string> args,Hardware* hardware);
void allocate(std::vector<std::string> args,Hardware* hardware);
void set(std::vector<std::string> args,Hardware* hardware);
void free(std::vector<std::string> args,Hardware* hardware);
void terminate(std::vector<std::string> args,Hardware* hardware);
void print(std::vector<std::string> args,Hardware* hardware);
int getStride(Mmu::Variable* var, Hardware* hardware);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory' and other hardware, stored in a struct for easy passing
    Hardware* hardware = new Hardware();

    hardware->page_table = new PageTable(page_size);
    hardware->mmu = new Mmu(67108864);
    hardware->memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    //std::vector<std::string> tokens;

    while (command != "exit") {

        std::istringstream buf(command); 
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> tokens(beg, end); 

        if( tokens.size() > 0){
        
            if(tokens[0] == "create")
            {
                create(tokens,hardware);
            }

            else if(tokens[0] == "allocate")
            {
                allocate(tokens,hardware);
            }

            else if(tokens[0] == "set")
            {
                set(tokens,hardware);
            }

            else if(tokens[0] == "free")
            {
                std::cout << "free" << std::endl;
            }

            else if(tokens[0] == "terminate")
            {
                std::cout << "terminate" << std::endl;
            }

            else if(tokens[0] == "print")
            {
                print(tokens, hardware);
            }

            else if(tokens[0] != "exit")
            {
                std::cout << "ERROR: unrecognized command" << std::endl;
            }

            // for(int i=0; i<tokens.size(); i++){
            //     std::cout << "\t " << tokens[i] << std::endl;
            // }
        }

        std::cout << "> ";
        std::getline (std::cin, command);
   }


    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void create(std::vector<std::string> args,Hardware* hardware)   //SHOULD BE WORKING
{
    if(args.size() != 3)
    {    
        std::cout << "Wrong number of arguments for command \"create\"" << std::endl;
        return;
    }

    int text;
    int data;

    try
    {
        text = std::stoi(args[1]);
        data = std::stoi(args[2]);
    }
    catch(std::invalid_argument)
    {
        std::cout << "ERROR: invalid argument to command \"create\"" << std::endl;
    }

    if(text < 2048 || text > 16384)
    {
        std::cout << "ERROR: text size must be between 2048 and 16384 bytes" << std::endl;
        return;
    }

    if(data < 0 || data > 1024)
    {
        std::cout << "ERROR: data size must be between 0 and 1024 bytes" << std::endl;
        return;
    }

    int pid = hardware->mmu->createProcess(text,data,hardware->page_table);

    std::cout << "New process created. PID: " << pid << std::endl;

    // create <text_size> <data_size>
        // Create a process
        // Assign a PID (start at 1024, increment from there)
        // Allocate some memory to begin
            // text 2048-16384 bytes
            // data 0-1024 bytes
            // stack constant 65536 bytes
        // Print process' PID
}

void allocate(std::vector<std::string> args,Hardware* hardware)
{
    if(args.size() != 5)
    {    
        std::cout << "Wrong number of arguments for command \"allocate\"" << std::endl;
        return;
    }

    try
    {
        int pid = std::stoi(args[1]);   //TODO pid might not correspond to a real process
        std::string name = args[2];
        
        Mmu::Datatype type;

        if(args[3] == "char")
            type = Mmu::Datatype::Char;

        else if(args[3] == "short")
            type = Mmu::Datatype::Short;

        else if(args[3] == "int")
            type = Mmu::Datatype::Int;

        else if(args[3] == "long")
            type = Mmu::Datatype::Long;

        else if(args[3] == "float")
            type = Mmu::Datatype::Float;

        else if(args[3] == "double")
            type = Mmu::Datatype::Double;

        else
        {
            std::cout << "ERROR: invalid datatype \"" << args[3] << "\"" << std::endl;
            return;
        }

        int n_elements = stoi(args[4]);

        hardware->mmu->allocateMemory(pid,name,type,n_elements,hardware->page_table);
    }

    catch(std::invalid_argument)
    {
        std::cout << "ERROR: invalid argument to command \"allocate\"" << std::endl;
    }

    // allocate <PID> <var_name> <data_type> <n_elements>
        // char: 1 byte per element
        // short: 2 bytes
        // int/float: 4 bytes
        // long/double: 8 bytes
}

void set(std::vector<std::string> args,Hardware* hardware)
{
    if(args.size() < 4)
    {
        std::cout << "ERROR: not enough arguments to command \"set\"" << std::endl;
        return;
    }

    int pid = std::stoi(args[1]);
    std::string var_name = args[2];
    int offset = std::stoi(args[3]);

    if(hardware->mmu->getProcess(pid) == NULL)
    {
        std::cout << "ERROR: process does not exist" << std::endl;
    }

    Mmu::Variable* var = hardware->mmu->getVariable(pid,var_name);

    if(var == NULL)
    {
        std::cout << "ERROR: variable does not exist" << std::endl;
    }

    int virtual_address = var->virtual_address;

    int physical_address = hardware->page_table->getPhysicalAddress(pid,virtual_address);
    // set <PID> <var_name> <offset> <value_0> <value_1> ... <value_n>
        // look up addresss of variable using page table
            // throw error if variable isn't real/user tries to buffer overflow(?)
        // change value of variable in the memory array

}

void free(std::vector<std::string> args,Hardware* hardware)
{
    //TODO implement function
}

void terminate(std::vector<std::string> args,Hardware* hardware)
{
    //TODO implement function
}

void print(std::vector<std::string> args,Hardware* hardware)
{
    bool error = false; 

    if(args.size() != 2){
        std::cout << "ERROR: wrong number of arguments" << std::endl;
    }

    // printing from hardware
    if(args[1] == "mmu"){

        hardware->mmu->print();
        //WORKS

    }

    else if(args[1] == "page"){

        hardware->page_table->print();
        //WORKS

    }

    else if(args[1] == "processes"){

        hardware->mmu->listProcesses();
        //WORKS 
    }

    //printing variable: <pid>:<var_name>
    else{

        int delimiter = args[1].find_first_of(':');
        if(delimiter == std::string::npos)    // if variable is not in proper format
        {
            std::cout << "ERROR: not a valid argument to command \"print\"" << std::endl;
            return;
        }

        int variable_pid = std::stoi(args[1].substr(0,delimiter));
        std::string variable_name = args[1].substr(delimiter+1,args[1].length() - delimiter);

        std::cout << "Process " << variable_pid << ", variable " << variable_name << std::endl;

        Mmu::Variable* var = hardware->mmu->getVariable(variable_pid,variable_name);
        
        if(var != NULL){

            //int stride = getStride(var, hardware);
            int addr = hardware->page_table->getPhysicalAddress( std::stoi(args[1]), var->virtual_address );    // TODO physical address isn't implemented yet

            if(var->type == "char"){
                for(int i=0; i<var->size; i++){

                    std::cout << char(hardware->memory[addr + i]) << ' ';
                }
            }

            else if(var->type == "short"){
                for(int i=0; i<var->size; i++){

                    std::cout << short(hardware->memory[addr + i*2]) << ' ';
                }
            }

            else if(var->type == "int"){
                for(int i=0; i<var->size; i++){

                    std::cout << int(hardware->memory[addr + i*4]) << ' ';
                }
            }

            else if(var->type == "float"){
                for(int i=0; i<var->size; i++){

                    std::cout << float(hardware->memory[addr + i*4]) << ' ';
                }
            }

            else if(var->type == "double"){
                for(int i=0; i<var->size; i++){

                    std::cout << double(hardware->memory[addr + i*8]) << ' ';
                }
            }

            else if(var->type == "long"){
                for(int i=0; i<var->size; i++){

                    std::cout << long(hardware->memory[addr + i*8]) << ' ';
                }
            }
        }

        else
        {
            std::cout << "ERROR: invalid argument to command \"print\"" << std::endl;   //TODO error checking might not be complete
        }
    }

}

int getStride(Mmu::Variable* var, Hardware* hardware){

    int stride;

    if(var->type == "char")
        stride = 1;

    else if(var->type == "short")
        stride = 2;

    else if(var->type == "int")
        stride = 4;

    else if(var->type == "float")
        stride = 4;

    else if(var->type == "double")
        stride = 8;

    else if(var->type == "long")
        stride = 8;

    return stride;
}