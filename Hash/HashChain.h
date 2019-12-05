#pragma once

#include <vector>
#include <list>
#include <set>
#include <memory>
#include <type_traits>

template< class Ty >
struct IChain
{
    virtual ~IChain() = default;
    virtual void add( const Ty& value ) = 0;
    virtual void remove( const Ty& value ) = 0;
    virtual bool contains( const Ty& value ) const = 0;
    virtual size_t size() const = 0;
    virtual void copyToIChain( IChain< Ty >& ic ) const = 0;
};

template< class Ty >
struct ChainBTS;

/// Лист
template< class Ty >
struct ChainList : public IChain< Ty >
{
    void add( const Ty& value ) override
    {
        m_data.push_back( value );
    }
    void remove( const Ty& value ) override
    {
        auto finded = std::find( std::begin( m_data ), std::end( m_data ), value );
        if( finded != m_data.end() )
        {
            m_data.erase( finded );
        }
    }
    bool contains( const Ty& value ) const override
    {
        auto finded = std::find( std::begin( m_data ), std::end( m_data ), value );
        return finded != m_data.end();
    }
    size_t size() const override
    {
        return m_data.size();
    }
    void copyToIChain( IChain< Ty >& ic ) const override
    {
        for( auto& v : m_data )
            ic.add( v );
    }
private:
    std::list< Ty > m_data;
};

/// Бинарное дерево
template< class Ty >
struct ChainBTS : public IChain< Ty >
{
    void add( const Ty& value ) override
    {
        m_data.insert( value );
    }
    void remove( const Ty& value ) override
    {
        m_data.erase( value );
    }
    bool contains( const Ty& value ) const override
    {
        return m_data.find( value ) != m_data.end();
    }
    size_t size() const override
    {
        return m_data.size();
    }
    void copyToIChain( IChain< Ty >& ic ) const override
    {
        for( auto& v : m_data )
            ic.add( v );
    }
private:
    std::set< Ty > m_data;
};

template< class Ty, class H = std::hash< Ty > >
class HashChain
{
public:
    HashChain( size_t size = 100 )
        : m_data( size )
    {
        // Сразу размещаем списки
        for( auto& v : m_data )
            v = std::make_unique< ChainList< Ty > >();
    }
    void add( const Ty& value )
    {
        // Берем позицию
        size_t pos = getPos( m_hash( value ) );

        // Число превысило порог в 32, копируем в дерево
        if( m_data[pos]->size() == 32 )
        {
            auto bts = std::make_unique< ChainBTS< Ty > >();
            m_data[pos]->copyToIChain( *bts );
            m_data[pos] = std::move(bts);
        }
        m_data[pos]->add( value );
    }

    void remove( const Ty& value )
    {
        size_t pos = getPos( m_hash( value ) );
        auto& c = m_data[pos];
        if( c == nullptr )
            return;

        c->remove( value );
        // Стало меньше 32, превращаем в список
        if( c->size() == 31 )
        {
            auto bts = std::make_unique< ChainList< Ty > >();
            m_data[pos]->copyToIChain( *bts );
            m_data[pos] = std::move(bts);
        }
    }
    bool contains( const Ty& value ) const
    {
        size_t pos = getPos( m_hash( value ) );
        return m_data[pos]->contains( value );
    }
private:
    size_t getPos( size_t h ) const
    {
        return h % m_data.size();
    }

    H m_hash;
    std::vector< std::unique_ptr< IChain< Ty > > > m_data;
};