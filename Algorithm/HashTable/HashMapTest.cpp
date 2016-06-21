#include "SeparateChainingHashMap.hpp"
#include "LinearProbingHashMap.hpp"
#include <assert.h>
#include <stdio.h>

int main()
{
    // ------------------------------------------------
    CYTL::SeparateChainingHashMap<std::string, int> hm;

    hm["cheukyin"] = 1;
    hm["yip"] = 2;
    hm["kkk"] = 3;

    assert(hm["cheukyin"] == 1);
    assert(hm["yip"] == 2);
    assert(hm["kkk"] == 3);

    hm.eraseKey("yip");

    assert( hm.isKeyExist("cheukyin") );
    assert( !hm.isKeyExist("yip") );


    // ------------------------------------------------
    CYTL::LinearProbingHashMap<std::string, int> lhm;

    lhm["cheukyin"] = 1;
    lhm["yip"] = 2;
    lhm["kkk"] = 3;
    lhm["aaa"] = 4;
    lhm["bbb"] = 5;
    lhm["ccc"] = 6;
    lhm["ddd"] = 7;
    lhm["eee"] = 8;

    assert(lhm["cheukyin"] == 1);
    assert(lhm["yip"] == 2);
    assert(lhm["kkk"] == 3);
    assert(lhm["aaa"] == 4);
    assert(lhm["bbb"] == 5);
    assert(lhm["ccc"] == 6);
    assert(lhm["ddd"] == 7);
    assert(lhm["eee"] == 8);

    lhm.eraseKey("bbb");
    lhm.eraseKey("yip");
    lhm.eraseKey("aaa");
    lhm.eraseKey("eee");

    assert(lhm["cheukyin"] == 1);
    assert( !lhm.isKeyExist("yip") );
    assert(lhm["kkk"] == 3);
    assert( !lhm.isKeyExist("aaa") );
    assert( !lhm.isKeyExist("bbb") );
    assert(lhm["ccc"] == 6);
    assert(lhm["ddd"] == 7);
    assert( !lhm.isKeyExist("eee") );

    lhm["eew"] = 80;
    lhm["ee3"] = 85;
    lhm["eae"] = 32;

    assert(lhm["eew"] == 80);
    assert(lhm["ee3"] == 85);
    assert(lhm["eae"] == 32);

    return 0;
}