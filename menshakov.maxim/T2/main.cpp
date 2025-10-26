#include "DataStruct.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <iterator>

int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    std::vector<DataStruct> v;

    std::istream_iterator<DataStruct> it(std::cin);
    std::istream_iterator<DataStruct> end;

    std::copy(it, end, std::back_inserter(v));

    std::sort(v.begin(), v.end(), data_struct_comparator);

    std::ostream_iterator<DataStruct> out_it(std::cout, "\n");
    std::copy(v.begin(), v.end(), out_it);

    return 0;
}
