#pragma once

#include "../config.h"
#include "IDPool.h"
#include "PoolHelper.h"
#include "Singleton.h"

#include <boost/intrusive/unordered_set.hpp>
#include <memory>

namespace carl::pool {

    template<class Content>
    class Pool;

    template<class Content>
    class PoolElementWrapper : public boost::intrusive::unordered_set_base_hook<> {
        friend Pool<Content>;

        std::size_t m_id;
        std::weak_ptr<PoolElementWrapper<Content>> m_weak_ptr;
        Content m_content;

    public:
        template <typename ...Args> 
        explicit PoolElementWrapper(Args && ...args) : m_content(std::forward<Args>(args)...) {}
        ~PoolElementWrapper() {
            Pool<Content>::getInstance().free(this);
        }

        const Content& content() const {
            return m_content;
        }

        auto id() const {
            return m_id;
        }
    };

    template<class Content>
    inline std::size_t hash_value(const PoolElementWrapper<Content>& wrapper) {
        return wrapper.content().key().hash();
    }

    template<class Content>
    bool operator==(const PoolElementWrapper<Content>& c1, const PoolElementWrapper<Content>& c2) {
        return c1.content().key() == c2.content().key();
    }
    
    template<class Content>
    class Pool : public Singleton<Pool<Content>>  {
        friend Singleton<Pool<Content>>;
        friend PoolElementWrapper<Content>;

        template<typename Key>
        struct content_equal {
            bool operator()(const Key& data, const PoolElementWrapper<Content>& content) const { 
                return data == content.content().key();
            }

            bool operator()(const PoolElementWrapper<Content>& content, const Key& data) const {
                return (*this)(data, content);
            }
        };

        template<typename Key>
        struct content_hash {
            std::size_t operator()(const Key& data) const {
                return data.hash();
            }
        };


        /// id allocator
        IDPool m_ids;
        //size_t mIdAllocator;
        pool::RehashPolicy m_rehash_policy;
        using UnderlyingSet = boost::intrusive::unordered_set<PoolElementWrapper<Content>>;
        std::unique_ptr<typename UnderlyingSet::bucket_type[]> m_pool_buckets;
        /// The pool.
        UnderlyingSet m_pool;
        
        #ifdef THREAD_SAFE
        /// Mutex to avoid multiple access to the pool
        mutable std::recursive_mutex m_mutex;
        #define DATASTRUCTURES_POOL_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(m_mutex);
        #define DATASTRUCTURES_POOL_LOCK m_mutex.lock();
        #define DATASTRUCTURES_POOL_UNLOCK m_mutex.unlock();
        #else
        #define DATASTRUCTURES_POOL_LOCK_GUARD
        #define DATASTRUCTURES_POOL_LOCK
        #define DATASTRUCTURES_POOL_UNLOCK
        #endif

        void check_rehash() {
            auto rehash = m_rehash_policy.needRehash(m_pool.bucket_count(), m_pool.size());
            if (rehash.first) {
                auto new_buckets = new typename UnderlyingSet::bucket_type[rehash.second];
                m_pool.rehash(typename UnderlyingSet::bucket_traits(new_buckets, rehash.second));
                m_pool_buckets.reset(new_buckets);
            }
        }

    protected:

        explicit Pool(std::size_t _capacity = 1000)
		: m_pool_buckets(new typename UnderlyingSet::bucket_type[m_rehash_policy.numBucketsFor(_capacity)]),
		  m_pool(typename UnderlyingSet::bucket_traits(m_pool_buckets.get(), m_rehash_policy.numBucketsFor(_capacity))) {
            m_ids.get();
            assert(m_ids.largestID() == 0);
        }

    public:

        ~Pool() {}

        template<typename Key>
        std::shared_ptr<PoolElementWrapper<Content>> add(Key&& c) {
            DATASTRUCTURES_POOL_LOCK_GUARD

            typename UnderlyingSet::insert_commit_data insert_data;
            auto res = m_pool.insert_check(c, content_hash<Key>(), content_equal<Key>(), insert_data);
            if (!res.second) {
                return res.first->m_weak_ptr.lock();
            } else {
                auto shared = std::make_shared<PoolElementWrapper<Content>>(std::move(c));
                shared.get()->m_id = m_ids.get();
                shared.get()->m_weak_ptr = shared;
                m_pool.insert_commit(*shared.get(), insert_data);
                check_rehash();
                return shared;
            }
        }

    protected:

        void free(const PoolElementWrapper<Content>* c) {
            if (c->id() != 0) {
                DATASTRUCTURES_POOL_LOCK_GUARD
                auto it = m_pool.find(*c);
                assert(it != m_pool.end());
                m_pool.erase(it);
                m_ids.free(c->id());
            }
        }
    };

    template<class Content>
    class PoolElement {
        std::shared_ptr<PoolElementWrapper<Content>> m_content;

    public:
        template<typename Key>
        PoolElement<Content>(Key&& k) : m_content(Pool<Content>::getInstance().add(std::move(k))) {}

        const Content& operator()() const {
            return m_content->content();
        }
        const Content& operator*() const {
            return m_content->content();
        }
        const Content* operator->() const {
            return &(m_content->content());
        }

        auto id() const {
            return m_content->id();
        }
    };


}