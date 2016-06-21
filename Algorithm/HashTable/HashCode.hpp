#ifndef __CYTL__HASHCODE__
#define __CYTL__HASHCODE__

#include <string.h>
#include <string>

namespace CYTL
{
    template<class T> struct HashCode
    {
        int operator()(const T& obj)
        { return obj.__hashcode__(); }
    };


    template<class T1, class T2>
    int hashCombine(const T1& t1, const T2& t2)
    { return HashCode<T1>()(t1) ^ (HashCode<T2>()(t2) << 1); }


    template<>
    struct HashCode<int>
    {
        int operator()(int i)
        { return i; }
    };

    template<>
    struct HashCode<double>
    {
        int operator()(const double i)
        {
            union{double d; size_t k;} u;
            u.d = i;
            size_t res = u.k;
            return res ^ (res >> 32);
        }
    };

    template<>
    struct HashCode<float>
    {
        int operator()(const float i)
        {
            union{float d; int k;} u;
            u.d = i;
            return u.k;
        }
    };

    template<>
    struct HashCode<bool>
    {
        int operator()(const bool b)
        { return b ? 1231 : 1237; }
    };

    template<>
    struct HashCode<std::string>
    {
        int operator()(const std::string& s)
        {
            int hash = 0;
            for(size_t i=0; i < s.size(); i++)
                hash += s[i] + 31 * hash;
            return hash;
        }
    };

    template<>
    struct HashCode<char*>
    {
        int operator()(const char* s)
        {
            int hash = 0;
            for(size_t i=0; i < strlen(s); i++)
                hash += s[i] + 31 * hash;
            return hash;
        }
    };

    template<>
    struct HashCode<const char*>
    {
        int operator()(const char* s)
        {
            int hash = 0;
            for(size_t i=0; i < strlen(s); i++)
                hash += s[i] + 31 * hash;
            return hash;
        }
    };

}

#endif // __CYTL__HASHCODE__