#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
    _used = new bool[67108864/_page_size];
}

PageTable::~PageTable()
{
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    if (_table.count(entry) == 0)
    {
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
            std::cout << "ERROR: no free space in memory" << std::endl;
        }
    }
}

int PageTable::getPhysicalAddress(uint32_t pid, int virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = virtual_address / _page_size;
    int page_offset = virtual_address % _page_size;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        address = _table[entry] + page_offset;
    }

    return address;
}


bool printSort(std::string x,std::string y)
{
    int delimiter = x.find_first_of('|');
    int x_pid = std::stoi(x.substr(0,delimiter));
    int x_page = std::stoi(x.substr(delimiter+1,x.length() - delimiter));

    delimiter = y.find_first_of('|');
    int y_pid = std::stoi(y.substr(0,delimiter));
    int y_page = std::stoi(y.substr(delimiter+1,y.length() - delimiter));

    if(x_pid != y_pid)
        return x_pid < y_pid;

    else
        return x_page < y_page;
}


void PageTable::print()
{
    std::vector<std::string> entries;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    for(auto it = _table.begin(); it != _table.end();it++)
    {
        entries.push_back(it->first);
    }

    std::sort(entries.begin(),entries.end(),printSort);

    for(auto it = entries.begin();it != entries.end();it++)
    {   
        std::string key = *it;

        int delimiter = key.find_first_of('|');
        int pid = std::stoi(key.substr(0,delimiter));
        int page = std::stoi(key.substr(delimiter+1,key.length() - delimiter));

        printf("%6d|%13d|%13d\n",pid,page,_table.find(key)->second);
    }
}

int PageTable::pageSize() const
{
    return _page_size;
}
