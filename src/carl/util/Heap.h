/**
 * @file Heap.h
 * This file has been extracted from mathic. It was deployed without license at:
 * http://www.broune.com/papers/issac2012.html
 * 
 * As of now, it is available at @cite Mathic .
 */

#pragma once

#include "CompactTree.h"

#include <ostream>
#include <string>
#include <vector>

namespace carl
{
    /** A heap priority queue.

        Configuration serves the same role as for Geobucket. It must have these
        fields that work as for Geobucket.

        * A type Entry
        * A type CompareResult
        * A const or static method: CompareResult compare(Entry, Entry)
        * A const or static method: bool cmpLessThan(CompareResult)
        * A static const bool supportDeduplication
        * A static or const method: bool cmpEqual(CompareResult)
        * A static or const method: Entry deduplicate(Entry a, Entry b)


        It also has these additional fields:

        * A static const bool fastIndex
        If this field is true, then a faster way of calculating indexes is used.
        This requires sizeof(Entry) to be a power of two! This can be achieved
        by adding padding to Entry, but this class does not do that for you.
    */
    template<class C>
    class Heap
    {
        public:
            class c_iterator;
            typedef C                             Configuration;
            typedef typename Configuration::Entry Entry;

            explicit Heap( const Configuration& configuration ):
				_tree(),
                _conf( configuration )
            {}
            Configuration& getConfiguration()
            {
                return _conf;
            }

            const Configuration& getConfiguration() const
            {
                return _conf;
            }

            std::string getName() const;
            void push( Entry entry );
            void push( const Entry* begin, const Entry* end );
            Entry pop();

            Entry top() const
            {
                return _tree[Node()];
            }

            bool empty() const
            {
                return _tree.empty();
            }

            size_t size() const
            {
                return _tree.size();
            }

            c_iterator begin() const
            {
                return c_iterator( _tree );
            }

            c_iterator end() const
            {
                return c_iterator( _tree, _tree.lastLeaf().next() );
            }

            std::vector<Entry> getCopy() const
            {
                _tree.getCopy();
            }

            void print( std::ostream& out = std::cout ) const;

            void decreaseTop( Entry newEntry );
            void decreasePos( Entry newEntry, c_iterator pos );

            void popPosition( c_iterator pos )
            {
                Entry movedValue = _tree[_tree.lastLeaf()];
                _tree.popBack();
                if( !_tree.empty() )
                    moveValueUp( moveHoleDown( pos.getNode() ), movedValue );

            }

            size_t getMemoryUse() const;

        private:
            typedef CompactTree<Entry, Configuration::fastIndex> Tree;
            typedef typename Tree::Node                      Node;

            Node moveHoleDown( Node hole );
            void moveValueDown( Node pos, Entry value );
            void moveValueUp( Node pos, Entry value );

            bool isValid() const;

            Tree          _tree;
            Configuration _conf;

        public:
            class c_iterator
            {
                public:
                    explicit c_iterator( const Tree& tree ):
                        mTree( tree ),
						pos()
                    {}

#ifdef __VS
                    c_iterator( const Tree& tree, Node startpos ):
#else
                    c_iterator(const Tree& tree, Heap::Node startpos) :
#endif
                        mTree( tree ),
                        pos( startpos )
                    {}

                    const Entry get() const
                    {
                        return mTree[pos];
                    }

                    void next()
                    {
                        pos = pos.next();
                    }

                    friend bool operator ==( c_iterator lhs, c_iterator rhs )
                    {
                        if( &lhs.mTree != &rhs.mTree )
                            return false;
                        return lhs.pos == rhs.pos;
                    }

                    friend bool operator !=( c_iterator lhs, c_iterator rhs )
                    {
                        return !(lhs == rhs);
                    }

                    const Node& getNode() const
                    {
                        return pos;
                    }

                protected:
#ifdef __VS
                    const Tree& mTree;
                    Node        pos;
#else
                    const Heap::Tree& mTree;
                    Heap::Node        pos;
#endif

            };

            using const_iterator = c_iterator;
    };

    template<class C>
    size_t Heap<C>::getMemoryUse() const
    {
        return _tree.getMemoryUse();
    }

    template<class C>
    std::string Heap<C>::getName() const
    {
        return std::string( "heap(" ) + (C::fastIndex ? "fi" : "si") + (C::supportDeduplicationWhileOrdering ? " dedup" : "") + ')';
    }

    template<class C>
    void Heap<C>::push( Entry entry )
    {
        assert( isValid() );
        _tree.pushBack( entry );
        moveValueUp( _tree.lastLeaf(), entry );
        assert( isValid() );
    }

    template<class C>
    void Heap<C>::push( const Entry* begin, const Entry* end )
    {
        for( ; begin != end; ++begin )
            push( *begin );
    }

    template<class C>
    void Heap<C>::decreaseTop( Entry newEntry )
    {
        moveValueUp( moveHoleDown( Node() ), newEntry );
        assert( isValid() );
    }

    template<class C>
    typename Heap<C>::Entry Heap<C>::pop()
    {
        Entry top        = _tree[Node()];
        Entry movedValue = _tree[_tree.lastLeaf()];
        _tree.popBack();
        if( !_tree.empty() )
            moveValueUp( moveHoleDown( Node() ), movedValue );

        return top;
        assert( isValid() );
    }

    template<class C>
    void Heap<C>::print( std::ostream& out ) const
    {
        out << getName() << _tree.size() << ": {" << _tree << "}\n";
    }

    template<class C>
    typename Heap<C>::Node Heap<C>::moveHoleDown( Node hole )
    {
        const Node firstWithout2Children = _tree.lastLeaf().next().parent();
        while( hole < firstWithout2Children )
        {
            // can assume hole has two children here
            Node child   = hole.left();
            Node sibling = child.next();
            if( _conf.cmpLessThan( _conf.compare( _tree[child], _tree[sibling] )))
                child = sibling;
            _tree[hole] = _tree[child];
            hole        = child;
        }
        // if we are at a node that has a single left child
        if( hole == firstWithout2Children && _tree.lastLeaf().isLeft() )
        {
            Node child = hole.left();
            _tree[hole] = _tree[child];
            return child;
        }
        return hole;
    }

    /*template<class C>
    void Heap<C>::moveValueDown(Node pos, Entry value) {
      const Node firstWithout2Children = _tree.lastLeaf().next().parent();
      while (pos < firstWithout2Children) {
        // can assume hole has two children here
        Node child = pos.left();
        Node sibling = child.next();
        if (_conf.cmpLessThan(_conf.compare(_tree[child], _tree[sibling])))
          child = sibling;
        if(_conf.cmpLessThan(_conf.compare(value, _tree[child]))) {
          _tree[pos] = _tree[child];
          pos = child;
        }
      }
      // if we are at a node that has a single left child
      if (pos == firstWithout2Children && _tree.lastLeaf().isLeft()) {
        Node child = pos.left();
        _tree[pos] = _tree[child];
        return child;
      }
      return hole;
    }*/

    template<class C>
    void Heap<C>::moveValueUp( Node pos, Entry value )
    {
        //const Node origPos = pos;
        while( !pos.isRoot() )
        {
            const Node                up  = pos.parent();
            typename C::CompareResult cmp = _conf.compare( _tree[up], value );
            if( _conf.cmpLessThan( cmp ))
            {
                _tree[pos] = _tree[up];
                pos        = up;
            }
            else if( C::supportDeduplicationWhileOrdering && _conf.cmpEqual( cmp ))
            {
                assert( false );
                //  bool elimToZero = _conf.deduplicate(_tree[up], value);
                //Since value now is smaller, we have to move it down in the tree..
                //  pos = moveHoleDown(pos);
                //if value is now empty
                //  if(value->empty()) {
                //      std::swap(_tree[pos], _tree[_tree.lastLeaf()]);
                //remove empty from the tree.
                //      _tree.popBack();
                //      return;
                //  }
                //otherwise we just move it up again.

                //  if(elimToZero) {
                //we do not do anything with this.
                //  }

                //  continue;
            }
            else
            {
                break;
            }
        }
        _tree[pos] = value;
        assert( isValid() );
    }

    template<class C>
    bool Heap<C>::isValid() const
    {
        assert( _tree.isValid() );
        //print();
        for( Node i = Node().next(); i <= _tree.lastLeaf(); ++i )
        {
            //      if(_conf.cmpLessThan(_conf.compare(_tree[i.parent()], _tree[i]))) {
            //          std::cout << i.parent()._index << ": ";
            //          _tree[i.parent()]->print();
            //          std::cout << std::endl;
            //          
            //          std::cout << i._index << ": ";
            //          _tree[i]->print();
            //          std::cout << std::endl;
            //          print();
            //      }
            assert( !_conf.cmpLessThan( _conf.compare( _tree[i.parent()], _tree[i] )));
        }
        return true;
    }
}
