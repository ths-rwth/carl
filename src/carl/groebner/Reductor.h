/** 
 * @file:   Reductor.h
 * @author: Sebastian Junges
 *
 * @since July 11, 2013
 */

#pragma once

#include "Ideal.h"
#include "ReductorEntry.h"
#include "../util/Heap.h"

namespace carl
{

/**
 *  Class with the settings for the reduction algorithm.
 */
template<class Polynomial>
class ReductorConfiguration
{
public:
    typedef ReductorEntry<Polynomial>* Entry;
    typedef typename Polynomial::Ordering Compare;

    static const CompareResult compare( Entry e1, Entry e2 )
    {
        return Compare::compare( e1->getLead( ), e2->getLead( ) );
    }

    static const bool cmpLessThan( CompareResult res )
    {
        return res == CompareResult::LESS;
    }
    static const bool supportDeduplicationWhileOrdering = false;

    static const bool cmpEqual( CompareResult res )
    {
        return res == CompareResult::EQUAL;
    }

    /**
     * should only be called if the compare result was EQUAL
     * eliminate duplicate leading monomials
     * @param e1 upper entry
     * @param e2 lower entry
     * @return true if e1->lt is cancelled
     */
    static const bool deduplicate( Entry e1, Entry e2 )
    {
        assert( Polynomial::Ordering::compare( e1->getLead( ), e2->getLead( ) ) == CompareResult::EQUAL );
        return e1->addCoefficient( e2->getLead( ).getCoeff( ) );
    }
    
    static const bool fastIndex = true;
};

 template<typename T>
struct hasOrigins
{
    const bool valid = true;
};

template<>
struct hasOrigins<void>
{
    const bool valid = false;
};

template<typename Polynomial, typename Origins, template <class> class Datastructure = carl::Heap, template <class> class Configuration = ReductorConfiguration>
class Reductor
{
   
    protected:
        typedef typename Polynomial::Order Order;
        typedef typename Configuration<Order>::Entry Entry;
        typedef typename Polynomial::Coeff Coeff;
public:

    
    Reductor( const MultivariateIdeal<Order>& ideal, const Polynomial& f ) :
    mIdeal( ideal ), mDatastruct( Configuration<Order>( ) ), mReductionOccured( false )
    {
        insert(f, Term<Coeff>( Coeff(1) ));
        if(hasOrigins<Origins>::valid)
        {
            mOrigins = f.getOrigins( );
        }
    }

    Reductor( const MultivariateIdeal<Order>& ideal, const Term<Coeff> f ) :
    mIdeal( ideal ), mDatastruct( Configuration<Order>( ) )
    {
        insert( f );
    }

    virtual ~Reductor( )
    {
    }

    /**
     * The basic reduce routine on a priority queue.
     * @return 
     */
    const bool reduce( )
    {
        while( !mDatastruct.empty( ) )
        {
            typename Configuration<Order>::Entry entry;
            std::shared_ptr<const Term<Coeff>> leadingTerm;
            // Find a leading term.
            do
            {
                // get actual leading term
                entry = mDatastruct.top( );
                leadingTerm = entry->getLead( );
                
                assert( leadingTerm.getCoeff( ) != 0 );
                // update the data structure.
                // only insert non-emty polynomials.
                if( !updateDatastruct( entry ) ) break;
                typename Configuration<Order>::Entry newentry = mDatastruct.top( );
                while( entry != newentry && Order::compare( leadingTerm, newentry->getLead( ) ) == CompareResult::EQUAL )
                {
//                    std::cout << newentry->getLead() << std::endl;
                    leadingTerm.addToCoeff( newentry->getLead( ).getCoeff( ) );
                    if( !updateDatastruct( newentry ) ) break;

                    newentry = mDatastruct.top( );
                }
//                mDatastruct.print();
            }
            while( leadingTerm.getCoeff( ) == 0 && !mDatastruct.empty( ) );
            // Done finding leading term.
            
            // We have found the leading term..
            if( leadingTerm.getCoeff( ) == 0 )
            {
                assert( mDatastruct.empty( ) );

                //LOGMSG_DEBUG( logger, "End reduction. Leading term is 0" );
                //then the datastructure is empty, we are done.
                return true;
            }

            //find a suitable reductor and the corresponding factor.
            DivisionLookupResult<Polynomial> divres(mIdeal.getDivisor( leadingTerm ));
            // check if the reduction succeeded.
            if( divres.success() )
            {
                mReductionOccured = true;
                if( PolConf::useOrigins ) mOrigins.calculateUnion( divisor.first.getOrigins( ) );
                insert( divisor.first.getTail( ), divisor.second );
            }
            else
            {
//                std::cout << leadingTerm << std::endl;
                // TODO we only need this if it is sorted 
                // assert( mRemainder.size( ) == 0 ? true : Order::compare( leadingTerm, mRemainder.back( )  ) != GREATER );
                mRemainder.push_back( leadingTerm );
                //LOGMSG_DEBUG( logger, "End reduction" );
                return false;
            }
            
        }
        //LOGMSG_DEBUG( logger, "End reduction" );
        return true;
    }

    /**
     * Gets the flag which indicates that a reduction has occurred  (p -> p' with p' != p)
     * @return the value of the flag
     */
    bool reductionOccured( )
    {
        return mReductionOccured;
    }

    /**
     * Uses the ideal to reduce a polynomial as far as possible.
     * @return 
     */
    Polynomial fullReduce( )
    {
        // TODO:
        // Do simple reductions first.
        //Log logger = getLog( "ginacra.mr.reduction" );
        //LOGMSG_DEBUG( logger, "Begin full reduction" );
        while( !reduce( ) )
        {

        }
        // TODO check whether this is sorted.
        Polynomial res( mRemainder.begin( ), mRemainder.end( ) );
//        std::cout << res << std::endl;
        if( PolConf::useOrigins )
        {
            res.setOrigins( mOrigins );
        }
        LOGMSG_DEBUG( logger, "End full reduction" );
        return res;
    }
private:

    /**
     * A small routine which updates the underlying data structure for the polynomial which is reduced.
     * @param entry
     * @return 
     */
    inline bool updateDatastruct( typename Configuration<Order>::Entry entry )
    {
        if( entry->getTail( ).isZero( ) )
        {
            mDatastruct.pop( );
            delete entry;
            if( mDatastruct.empty( ) ) return false;
        }
        else
        {
            //mDatastruct.print();
            //std::cout << "replace lt" << std::endl;
            entry->replaceLeadTerm( );

            mDatastruct.decreaseTop( entry );
        }
        return true;
    }

    void insert( const Polynomial& g, const Term<Coeff>& fact )
    {
        if(!g.isZero())
        {
            mDatastruct.push( Entry( fact, g ) );
        }
    }

    void insert( const Term<Coeff>& g )
    {
        assert( g.getCoeff( ) != 0 );
        mDatastruct.push( new Entry ( g ) );
    }



    const MultivariateIdeal<Order>& mIdeal;
    Datastructure<Configuration<Order> > mDatastruct;
    std::vector<std::shared_ptr<const Term<Coeff>>> mRemainder;
    bool mReductionOccured;
    Origins mOrigins;
};


}
