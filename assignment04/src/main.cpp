#include <iostream>
#include <string>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void tokenize(std::string const &str,const char delim,std::vector<std::string> &out);
void create(std::vector<std::string> args);
void allocate(std::vector<std::string> args);
void set(std::vector<std::string> args);

typedef struct Hardware
{
	PageTable page_table;
	Mmu mmu;
	uint8_t* memory;
} Hardware;

Hardware* hardware; // global pointer to hardware struct

int main(int argc, char **argv)
{
	std::cout << sizeof(Datatype) << std::endl;
	// Ensure user specified page size as a command line parameter
	if (argc < 2)
	{
		fprintf(stderr, "Error: you must specify the page size\n");
		return 1;
	}

	// Print opening instuction message
	int page_size = std::stoi(argv[1]);
	printStartMessage(page_size);

	// Create physical 'memory' and other hardware, then make global
	Hardware* hardware_pointer;

	hardware_pointer->page_table = PageTable(page_size);
	hardware_pointer->mmu = Mmu(67108864);
	hardware_pointer->memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

	hardware = hardware_pointer;

	// Prompt loop
	std::string command;
	std::vector<std::string> args;

	std::cout << "> ";
	std::getline (std::cin, command);

	while (command != "exit")
	{
		tokenize(command,' ',args);

		if(args[0] == "create")
		{
			create(args);
		}

		else if(args[0] == "allocate")
		{
			allocate(args);
		}

		else if(args[0] == "set")
		{
			set(args);
		}

		else if(args[0] == "free")
		{
			std::cout << "free" << std::endl;
		}

		else if(args[0] == "terminate")
		{
			std::cout << "terminate" << std::endl;
		}

		else if(args[0] == "print")
		{
			std::cout << "print" << std::endl;
		}

		std::cout << "> ";
		std::getline (std::cin, command);

		args.clear();
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

void tokenize(std::string const &str,const char delim,std::vector<std::string> &out)
{// SRC: https://www.techiedelight.com/split-string-cpp-using-delimiter/
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}

void create(std::vector<std::string> args)
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

	int pid = hardware->mmu.createProcess(text,data);

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

void allocate(std::vector<std::string> args)
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
		
		Datatype type;

		if(args[3] == "char")
			type = Datatype::Char;

		else if(args[3] == "short")
			type = Datatype::Short;

		else if(args[3] == "int")
			type = Datatype::Int;

		else if(args[3] == "long")
			type = Datatype::Long;

		else if(args[3] == "float")
			type = Datatype::Float;

		else if(args[3] == "double")
			type = Datatype::Double;

		else
		{
			std::cout << "Invalid datatype" << std::endl;
			return;
		}

		int n_elements = stoi(args[4]);
	}

	catch(std::invalid_argument)
	{
		std::cout << "ERROR: invalid argument to command \"allocate\"" << std::endl;
	}

	//TODO actually allocate the data

	// allocate <PID> <var_name> <data_type> <n_elements>
		// char: 1 byte per element
		// short: 2 bytes
		// int/float: 4 bytes
		// long/double: 8 bytes
}

void set(std::vector<std::string> args)
{
	//TODO figure out how to error check this

	// set <PID> <var_name> <offset> <value_0> <value_1> ... <value_n>
		// look up addresss of variable using page table
			// throw error if variable isn't real/user tries to buffer overflow(?)
		// change value of variable in the memory array

}