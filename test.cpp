#include"list.h"
#include<vector>
#include<set>
#include<algorithm>
#include<functional>
#include<iostream>
#include<cstdio>
using namespace std;
int main(){
/*
    int ia[5] = {0, 1, 2, 3, 4};
    unsigned int i;
    //vector<int, zzf_stl::default_alloc> iv(ia, ia + 5);
    vector<int, zzf_stl::simple_alloc<int, zzf_stl::default_alloc>> iv(ia, ia + 5);
    for (i = 0; i < 10000; i++)
        iv.push_back(ia[i % 5]);
        */
    zzf_stl::list<int> li;
    li.push_back(1);
    li.push_back(2);
    li.push_back(3);
    li.push_back(4);
    li.push_back(5);
    li.push_back(6);
    li.push_back(7);
    li.push_back(8);
    li.push_back(9);
    li.push_back(10);
    auto iter1 = li.find(li.begin(), li.end(), 5);
    auto iter2 = li.find(li.begin(), li.end(), 9);
    auto pos = li.find(li.begin(), li.end(), 2);
    //li.transfer(pos, iter1, iter2);
    li.void_test_out();
    return 0;
    
    //printf("%x\n", 1987511984);
}

