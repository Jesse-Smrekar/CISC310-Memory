#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
    _used = new bool[67108864/_page_size];  // stores whether a particular frame is currently being used
}

PageTable::~PageTable()
{
}

//NOT TESTED YET
void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // Find free frame
    int frame = 0;
    bool found = false;

    for(frame;frame < (67108864/_page_size);frame++)
    {
        if(_used[frame] == false)
        {
            found = true;
            break;
        }
    }

    if(found)
    {
        _table[entry] = frame;
        _used[frame] = true;
    }

    else
    {
        std::cout << "ERROR: PageTable::addEntry couldn't find a free space" << std::endl;  //TODO maybe use this to throw memory full error?
    }
}

int PageTable::getPhysicalAddress(uint32_t pid, int virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        // TODO: implement this!
    }

    return address;
}

void PageTable::print()
{//TODO prints in STRING sorted order, not INT sorted order
    std::map<std::string, int>::iterator it;
    std::vector<std::vector<int>> entries;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    for (it = _table.begin(); it != _table.end(); it++)
    {
        std::string key = it->first;

        int delimiter = key.find_first_of('|');
        int pid = std::stoi(key.substr(0,delimiter));
        int page = std::stoi(key.substr(delimiter+1,key.length() - delimiter));

        int frame = it->second;

        printf("%6d|%13d|%13d\n",pid,page,frame);
   }
}

int PageTable::pageSize() const
{
    return _page_size;
}