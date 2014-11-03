///**
// * @file Pool.h
// * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
// */
//
//#pragma once
//
//#include "../formula/Common.h"
//#include "Singleton.h"
//
//namespace carl
//{
//    template<typename Element>
//    class Pool: public Singleton<Pool<Element>>
//    {
//        friend Singleton<Pool>;
//        
//        private:
//            // Members:
//            /// id allocator
//            unsigned mIdAllocator;
//            /// The pool.
//            FastPointerSet<Element> mPool;
//            /// Mutex to avoid multiple access to the pool
//            mutable std::mutex mMutex;
//            
//            #define POOL_LOCK_GUARD std::lock_guard<std::mutex> lock( mMutex );
//            #define POOL_LOCK mMutexPool.lock();
//            #define POOL_UNLOCK mMutexPool.unlock();
//            
//        protected:
//            
//            /**
//             * Constructor of the pool.
//             * @param _capacity Expected necessary capacity of the pool.
//             */
//            Pool( unsigned _capacity = 10000 );
//            
//            void setId( Element& _element ) const;
//            
//            void init( Element& _element ) const;
//            
//            void postProcess( Element* _element );
//            
//        public:
//
//            /**
//             * Destructor.
//             */
//            ~Pool();
//            
//            const Element* add( Element* _element )
//            {
//                POOL_LOCK_GUARD
//                auto iterBoolPair = mAsts.insert( _element );
//                if( !iterBoolPair.second ) // Element has already been generated.
//                {
//                    delete _element;
//                }
//                else
//                {
//                    setId( *_element ); // id should be set here to avoid conflicts when multi-threading
//                    ++mIdAllocator;
//                    init( *_element );
//                    postProcess( _element );
//                }
//                return *iterBoolPair.first;   
//            };
//    
//} // end namespace carl