#include "hash_table.h"

using namespace std;

int main() {
    vector<int>input_vec = { 1, 1, 1, 1, 2, 2, 2, 3, 3, 4};
    print_numbers_count(count_numbers(input_vec), input_vec.size());
    return 0;
}
