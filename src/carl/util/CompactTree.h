/**
 * @file CompactTree.h
 * This file has been extracted from mathic. It was deployed without license at:
 * http://www.broune.com/papers/issac2012.html
 * Currently, it is distributed under LGPL v2  from 
 * http://www.broune.com/papers/issac2012.html
 */


#pragma once
#include <ostream>
#include <cassert>


namespace carl
{
    /** This class packs a complete binary tree in a vector.

        The idea is to have the root at index 1, and then the left child of node n
        will be at index 2n and the right child will be at index 2n + 1. The
        corresponding formulas when indexes start at 0 take more computation, so
        we need a 1-based array so we can't use std::vector.

        Also, when sizeof(Entry) is a power of 2 it is faster to keep track of
        i * sizeof(Entry) than directly keeping track of an index i. This doesn't
        work well when sizeof(Entry) is not a power of two. So we need both
        possibilities. That is why this class never exposes indexes. Instead
        you interact with Node objects that serve the role of an index, but the
        precise value it stores is encapsulated. This way you can't do something
        like _array[i * sizeof(Entry)] by accident. Client code also does not
        need to (indeed, can't) be aware of how indexes are calculated, stored and
        looked up.

        If FastIndex is false, then Nodes contain an index i. If FastIndex is
        true, then Nodes contain the byte offset i * sizeof(Entry). FastIndex must
        be false if sizeof(Entry) is not a power of two.
    */
    template<class Entry, bool FastIndex>
    class CompactTree
    {
        public:
            class Node;
            explicit CompactTree( std::size_t initialCapacity = 0 );
            CompactTree( const CompactTree& tree, std::size_t minCapacity = 0 );
            ~CompactTree()
            {
                delete[] (_array + 1);
            }

            Entry& operator []( Node n );
            const Entry& operator []( Node n ) const;

            bool empty() const
            {
                return _lastLeaf == Node( 0 );
            }

            std::size_t size() const
            {
                return _lastLeaf.getNormalIndex();
            }

            std::size_t capacity() const
            {
                return _capacityEnd.getNormalIndex();
            }

            Node lastLeaf() const
            {
                return _lastLeaf;
            }

            void pushBack( const Entry& value );
            void pushBackWithCapacity( const Entry& value );
            void popBack();

            bool hasFreeCapacity( size_t extraCapacity ) const;
            void increaseCapacity();
            void swap( CompactTree& tree );

            class Node
            {
			public:
                Node():
                    _index( fi ? S : 1 )
                {}    // the root node is the default

                Node parent() const;
                Node left() const;
                Node right() const;
                Node sibling() const;
                Node leftSibling() const;
                Node next( size_t count = 1) const;
                Node prev() const;

                Node& operator ++()
                {
                    *this = next();
                    return *this;
                }

                bool isRoot() const
                {
                    return *this == Node();
                }

                bool isLeft() const
                {
                    return fi ? !(_index & S) : !(_index & 1);
                }

                bool isRight() const
                {
                    return fi ? _index & S : _index & 1;
                }

                bool operator <( Node node ) const
                {
                    return _index < node._index;
                }

                bool operator <=( Node node ) const
                {
                    return _index <= node._index;
                }

                bool operator >( Node node ) const
                {
                    return _index > node._index;
                }

                bool operator >=( Node node ) const
                {
                    return _index >= node._index;
                }

                bool operator ==( Node node ) const
                {
                    return _index == node._index;
                }

                bool operator !=( Node node ) const
                {
                    return _index != node._index;
                }

            //private:
                friend class CompactTree<Entry, FastIndex>;
                static const bool   fi = FastIndex;
                static const size_t S  = sizeof(Entry);

                explicit Node( size_t i ):
                    _index( i )
                {}
                size_t getNormalIndex() const
                {
                    return fi ? _index / S : _index;
                }

                size_t _index;
            };

            void print( std::ostream& out ) const;

            void clear();

            std::size_t getMemoryUse() const;

            bool isValid() const;

            std::vector<Entry> getCopy() const
            {
                Node it;
                std::vector<Entry> ret;
                while( it <= _lastLeaf )
                {
                    ret.push_back( (*this)[it] );
                }
                return ret;
            }

        private:
            CompactTree& operator = ( const CompactTree& tree ) const;    // not available

            Entry* _array;
            Node   _lastLeaf;
            Node   _capacityEnd;
    };

    template<class E, bool FI>
    void CompactTree<E, FI>::clear()
    {
        _lastLeaf = Node( 0 );
    }

    template<class E, bool FI>
    size_t CompactTree<E, FI>::getMemoryUse() const
    {
        return capacity() * sizeof(E);
    }

    template<class E, bool FI>
    std::ostream& operator <<( std::ostream& out, const CompactTree<E, FI>& tree )
    {
        tree.print( out );
        return out;
    }

    template<class E, bool FI>
    CompactTree<E, FI>::CompactTree( size_t initialCapacity ):
        _array( static_cast<E*>(nullptr) - 1 ),
        _lastLeaf( 0 ),
        _capacityEnd( Node( 0 ).next( initialCapacity ))
    {
        if( initialCapacity > 0 )
            _array = new E[initialCapacity]-1;
        assert( isValid() );
    }

    template<class E, bool FI>
    CompactTree<E, FI>::CompactTree( const CompactTree& tree, size_t minCapacity ):
        _array( static_cast<E*>(0) - 1 ),
        _lastLeaf( tree._lastLeaf )
    {
        if( tree.size() > minCapacity )
            minCapacity = tree.size();
        _capacityEnd = Node( 0 ).next( minCapacity );
        if( minCapacity == 0 )
            return;
        _array = new E[minCapacity]-1;
        for( Node i; i <= tree.lastLeaf(); ++i )
            (*this)[i] = tree[i];
    }

    template<class E, bool FI>
    E& CompactTree<E, FI>::operator []( Node n )
    {
        if( !FI )
            return _array[n._index];
        char* base    = reinterpret_cast<char*>(_array);
        E*    element = reinterpret_cast<E*>(base + n._index);
        assert( element == &(_array[n._index / sizeof(E)]) );
        return *element;
    }

    template<class E, bool FI>
    const E& CompactTree<E, FI>::operator []( Node n ) const
    {
        return const_cast<CompactTree<E, FI>*>(this)->operator []( n );
    }

    template<class E, bool FI>
    void CompactTree<E, FI>::pushBack( const E& value )
    {
        if( _lastLeaf == _capacityEnd )
            increaseCapacity();
        _lastLeaf           = _lastLeaf.next();
        (*this)[lastLeaf()] = value;
    }

    template<class E, bool FI>
    void CompactTree<E, FI>::pushBackWithCapacity( const E& value )
    {
        assert( _lastLeaf != _capacityEnd );
        _lastLeaf           = _lastLeaf.next();
        (*this)[lastLeaf()] = value;
    }

    template<class E, bool FI>
    void CompactTree<E, FI>::popBack()
    {
        assert( _lastLeaf >= Node() );
        _lastLeaf = _lastLeaf.prev();
    }

    template<class E, bool FI>
    void CompactTree<E, FI>::swap( CompactTree& tree )
    {
        std::swap( _array, tree._array );
        std::swap( _lastLeaf, tree._lastLeaf );
        std::swap( _capacityEnd, tree._capacityEnd );
    }

    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::parent() const
    {
        return fi ? Node( (_index / (2 * S)) * S ) : Node( _index / 2 );
    }

    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::left() const
    {
        return Node( 2 * _index );
    }

    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::right() const
    {
        return fi ? Node( 2 * _index + S ) : Node( 2 * _index + 1 );
    }

    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::sibling() const
    {
        return fi ? Node( _index ^ S ) : Node( _index ^ 1 );
    }

    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::leftSibling() const
    {
        return fi ? Node( _index & ~S ) : Node( _index & ~1 );
    }


    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::next( size_t count ) const
    {
        return fi ? Node( _index + S * count ) : Node( _index + count );
    }

    template<class E, bool FI>
    typename CompactTree<E, FI>::Node CompactTree<E, FI>::Node::prev() const
    {
        return fi ? Node( _index - S ) : Node( _index - 1 );
    }

    template<class E, bool FI>
    void CompactTree<E, FI>::print( std::ostream& out ) const
    {
        Node last = lastLeaf();
        for( Node i; i <= last; i = i.next() )
        {
            if( (i._index & (i._index - 1)) == 0 )    // if i._index is a power of 2
                out << "\n " << i._index << ':';
            out << ' ' << (*this)[i] << " := ";
            out.flush();
            (*this)[i]->print( out );
        }
        out << "}\n";
    }

    template<class E, bool FI>
    bool CompactTree<E, FI>::isValid() const
    {
        // sizeof(Entry) must be a power of two if FastIndex is true.
        assert( !FI || (sizeof(E) & (sizeof(E) - 1)) == 0 );
        if( capacity() == 0 )
        {
            assert( _array == static_cast<E*>(nullptr) - 1 );
            assert( _capacityEnd == Node( 0 ));
            assert( _lastLeaf == Node( 0 ));
        }
        else
        {
            assert( _array != static_cast<E*>(nullptr) - 1 );
            assert( _capacityEnd > Node( 0 ));
            assert( _lastLeaf <= _capacityEnd );
        }
        return true;
    }

    template<class E, bool FI>
    bool CompactTree<E, FI>::hasFreeCapacity( size_t extraCapacity ) const
    {
        return Node( _capacityEnd._index - _lastLeaf._index ) >= Node( 0 ).next( extraCapacity );
    }

    template<class E, bool FI>
    void CompactTree<E, FI>::increaseCapacity()
    {
        CompactTree<E, FI> newTree( capacity() == 0 ? 16 : capacity() * 2 );
        for( Node i; i <= lastLeaf(); i = i.next() )
            newTree.pushBack( (*this)[i] );
        std::swap( _array, newTree._array );
        std::swap( _capacityEnd, newTree._capacityEnd );
        assert( isValid() );
    }
}
