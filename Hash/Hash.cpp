#include <iostream>
#include <random>
#include <numeric>

#include "HashChain.h"

void Test()
{
    std::vector< int > test( size_t( 10000 ) );
    std::iota( test.begin(), test.end(), 0 );
    std::mt19937 mt;
    std::shuffle( test.begin(), test.end(), mt );

    HashChain< int > hc;
    for( auto& v : test )
        hc.add( v );

    for( auto& v : test )
        if( !hc.contains( v ) )
            throw std::runtime_error( "fail test add!" );

    for( auto& v : test )
        hc.remove( v );

    for( auto& v : test )
        if( hc.contains( v ) )
            throw std::runtime_error( "fail test remove!" );

    for( auto& v : test )
        hc.add( v );

    for( auto& v : test )
        if( !hc.contains( v ) )
            throw std::runtime_error( "fail test add!" );
}

int main()
{
    try
    {
        Test();
    }
    catch( const std::exception& err )
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
