#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdlib.h>

typedef unsigned long long int uint64;

// klnk + k(lnlnk − 1) < pk < klnk + klnlnk for k ≥ 6
uint64 findKthPrime(uint64 k) // k >= 6
{
    uint64 range = k*log(k) + k*log(log(k));
    char *notPrime = (char*)calloc(range+1, sizeof(char));
    uint64 primeCnt = 1, prime = 2;

    while(primeCnt++ < k)
    {
        for(uint64 i = prime*prime; i <= range; i += prime)
            notPrime[i] = true;
        prime = std::find(notPrime+prime+1, notPrime+1+range, false) - notPrime;
    }

    return prime;
}

int main()
{
    std::cout << findKthPrime(4263116) << std::endl;
    return 0;
}