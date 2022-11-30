#pragma once

#include "../config.h"
#include "IDPool.h"
#include "PoolHelper.h"

#include <boost/intrusive/unordered_set.hpp>
#include <memory>

namespace carl::pool {

    template<class Content>
    class LocalPool;

    template<class Content>
    class LocalPoolElementWrapper : public boost::intrusive::unordered_set_base_hook<> {
        friend LocalPool<Content>;

        std::size_t m_id;
        std::weak_ptr<LocalPoolElementWrapper<Content>> m_weak_ptr;
        std::shared_ptr<LocalPool<Content>> m_pool;
        Content m_content;

    public:
        template <typename ...Args> 
        explicit LocalPoolElementWrapper(std::shared_ptr<LocalPool<Content>> pool, Args && ...args) : m_pool(pool), m_content(std::forward<Args>(args)...) {}
        ~LocalPoolElementWrapper() {
            m_pool->free(this);
        }

        const Content& content() const {
            return m_content;
        }

        auto id() const {
            return m_id;
        }
    };

    template<class Content>
    inline std::size_t hash_value(const LocalPoolElementWrapper<Content>& wrapper) {
        return wrapper.content().key().hash();
    }

    template<class Content>
    bool operator==(const LocalPoolElementWrapper<Content>& c1, const LocalPoolElementWrapper<Content>& c2) {
        return c1.content().key() == c2.content().key();
    }
    
    template<class Content>
    class LocalPool {
        friend LocalPoolElementWrapper<Content>;

        template<typename Key>
        struct content_equal {
            bool operator()(const Key& data, const LocalPoolElementWrapper<Content>& content) const { 
                return data == content.content().key();
            }

            bool operator()(const LocalPoolElementWrapper<Content>& content, const Key& data) const {
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
        using UnderlyingSet = boost::intrusive::unordered_set<LocalPoolElementWrapper<Content>>;
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

    public:
        LocalPool(std::size_t _capacity = 1000)
		: m_pool_buckets(new typename UnderlyingSet::bucket_type[m_rehash_policy.numBucketsFor(_capacity)]),
		  m_pool(typename UnderlyingSet::bucket_traits(m_pool_buckets.get(), m_rehash_policy.numBucketsFor(_capacity))) {
            m_ids.get();
            assert(m_ids.largestID() == 0);
        }

        ~LocalPool() {}

        template<typename Key>
        std::shared_ptr<LocalPoolElementWrapper<Content>> add(std::shared_ptr<LocalPool<Content>> pool, Key&& c) {
            DATASTRUCTURES_POOL_LOCK_GUARD
            assert(pool.get() == this);

            typename UnderlyingSet::insert_commit_data insert_data;
            auto res = m_pool.insert_check(c, content_hash<Key>(), content_equal<Key>(), insert_data);
            if (!res.second) {
                return res.first->m_weak_ptr.lock();
            } else {
                auto shared = std::make_shared<LocalPoolElementWrapper<Content>>(pool, std::move(c));
                shared.get()->m_id = m_ids.get();
                shared.get()->m_weak_ptr = shared;
                m_pool.insert_commit(*shared.get(), insert_data);
                check_rehash();
                return shared;
            }
        }

    protected:

        void free(const LocalPoolElementWrapper<Content>* c) {
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
    class LocalPoolElement {
        std::shared_ptr<LocalPoolElementWrapper<Content>> m_content;

    public:
        template<typename Key>
        LocalPoolElement(std::shared_ptr<LocalPool<Content>>& pool, Key&& k) : m_content(pool->add(pool, std::move(k))) {}

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

        bool operator==(const LocalPoolElement& other) const {
            return m_content == other.m_content;
        }
    };


}