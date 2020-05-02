#ifndef __PAGETABLE_H_
#define __PAGETABLE_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

class PageTable {
private:
    int _page_size;
    std::map<std::string, int> _table;
    bool* _used;  // stores whether a particular frame is currently being used

public:
    PageTable(int page_size);
    ~PageTable();

    void addEntry(uint32_t pid, int page_number);
    int getPhysicalAddress(uint32_t pid, int virtual_address);
    void print();

    int pageSize() const;
};

#endif // __PAGETABLE_H_
