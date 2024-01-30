#include"alloc.h"
#include<vector>
#include<algorithm>
#include<functional>
#include<iostream>

using namespace std;
int main(){
    
    int ia[5] = {0, 1, 2, 3, 4};
    unsigned int i;

    vector<int, zzf_stl::simple_alloc<int, zzf_stl::malloc_alloc>> iv(ia, ia + 5);
    for (i = 0; i < 5; i++)
        std::cout << iv[i] << std::endl;
    return 0;
}

