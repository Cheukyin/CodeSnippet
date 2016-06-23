#include "UnionFind.hpp"
#include <assert.h>

int main()
{
    CYTL::UnionFind uf(10);

    assert( !uf.isConnected(2, 4) );

    uf.connect(4, 8);
    assert( uf.isConnected(8, 4) );
    assert( !uf.isConnected(5, 4) );

    uf.connect(1, 9);
    assert( uf.isConnected(1, 9) );
    assert( !uf.isConnected(1, 8) );

    uf.connect(1, 4);
    assert( uf.isConnected(1, 8) );
    assert( uf.isConnected(1, 4) );
    assert( uf.isConnected(4, 9) );
    assert( uf.isConnected(8, 9) );
    assert( !uf.isConnected(8, 2) );

    return 0;
}
