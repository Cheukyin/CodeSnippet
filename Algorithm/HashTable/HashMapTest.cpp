#include "SeparateChainingHashMap.hpp"
#include <assert.h>
#include <stdio.h>

int main()
{
    CYTL::SeparateChainingHashMap<std::string, int> hm;

    hm["cheukyin"] = 1;
    hm["yip"] = 2;
    hm["kkk"] = 3;

    assert(hm["cheukyin"] == 1);
    assert(hm["yip"] == 2);
    assert(hm["kkk"] == 3);

    hm.eraseKey("yip");
    assert( !hm.isKeyExist("yip") );

    return 0;
}










