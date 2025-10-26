#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

struct DataStruct {
    long long key1;
    std::pair<long long, unsigned long long> key2;
    std::string key3;
};

std::istream& operator>>(std::istream& is, DataStruct& ds);
std::ostream& operator<<(std::ostream& os, const DataStruct& ds);

bool data_struct_comparator(const DataStruct& a, const DataStruct& b);

bool readDataStruct(std::istream& in, DataStruct& ds);

#endif
