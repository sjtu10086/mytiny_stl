#include"alloc.h"
#include<vector>
#include<set>
#include<algorithm>
#include<functional>
#include<iostream>

using namespace std;
int main(){
    int ia[5] = {0, 1, 2, 3, 4};
    unsigned int i;
    set<int> s;
    vector<int, zzf_stl::simple_alloc<int, zzf_stl::default_alloc>> iv(ia, ia + 5);
    for (i = 0; i < 10000; i++)
        iv.push_back(ia[i % 5]);
    return 0;
}

