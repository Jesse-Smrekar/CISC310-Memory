#include <iostream>
#include <string>
#include <bits/stdc++.h> 
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void create(std::vector<std::string> args);
void allocate(std::vector<std::string> args);

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

    // Create physical 'memory'
    uint8_t *memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

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
                create(tokens);
            }

            else if(tokens[0] == "allocate")
            {
                allocate(tokens);
            }

            else if(tokens[0] == "set")
            {
                std::cout << "set" << std::endl;
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
                std::cout << "print" << std::endl;
            }

            for(int i=0; i<tokens.size(); i++){
                std::cout << "\t " << tokens[i] << std::endl;
            }
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

void create(std::vector<std::string> args){


}
void allocate(std::vector<std::string> args){



}