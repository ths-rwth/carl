/**
 * @file UFManager.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once


#include "../../util/Common.h"
#include "../../util/hash.h"
#include "../../util/Singleton.h"
#include "../Sort.h"
#include "UninterpretedFunction.h"

#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace carl
{

// Forward declaration.
class UFManager;

/**
 * The actual content of an uninterpreted function instance.
 */
class UFContent {
    friend class UFManager;

    private:
        /// The uninterpreted function's name.
        std::string mName;
        /// The uninterpreted function's domain.
        std::vector<Sort> mDomain;
        /// The uninterpreted function's codomain.
        Sort mCodomain;

    public:
        /**
         * Constructs the content of an uninterpreted function.
         * @param name The name of the uninterpreted function to construct.
         * @param domain The domain of the uninterpreted function to construct.
         * @param codomain The codomain of the uninterpreted function to construct.
         */
        explicit UFContent(std::string&& name, std::vector<Sort>&& domain, Sort codomain):
            mName(std::move(name)),
            mDomain(std::move(domain)),
            mCodomain(codomain)
        {}
        UFContent() = delete;
        UFContent(const UFContent&) = delete;
        UFContent(UFContent&&) = delete;

        /**
         * @return The name of the uninterpreted function.
         */
        const std::string& name() const {
            return mName;
        }

        /**
         * @return The domain of the uninterpreted function.
         */
        const std::vector<Sort>& domain() const {
            return mDomain;
        }

        /**
         * @return The codomain of the uninterpreted function.
         */
        const Sort& codomain() const {
            return mCodomain;
        }

        /**
         * @param ufc The uninterpreted function's content to compare with.
         * @return true, if this uninterpreted function's content is less than the given one.
         */
        bool operator==(const UFContent& ufc) const {
            return mDomain == ufc.domain() && mName == ufc.name();
        }

        /**
         * @param ufc The uninterpreted function's content to compare with.
         * @return true, if this uninterpreted function's content is less than the given one.
         */
        bool operator<(const UFContent& ufc) const {
            if(mName < ufc.name()) {
                return true;
            }
            if(mName > ufc.name()) {
                return false;
            }
            if(mDomain.size() < ufc.domain().size()) {
                return true;
            }
            if(mDomain.size() > ufc.domain().size()) {
                return false;
            }
            auto domA = mDomain.begin();
            auto domB = ufc.domain().begin();
            while(domA != mDomain.end()) {
                assert(domB != ufc.domain().end());
                if(*domA < *domB) {
                    return true;
                }
                if(*domB < *domA) {
                    return false;
                }
                ++domA;
                ++domB;
            }
            return false;
        }
};
} // end namespace carl

namespace std
{
/**
 * Implements std::hash for uninterpreted function's contents.
 */
template<>
struct hash<carl::UFContent> {
  public:
    /**
     * @param ufun The uninterpreted function to get the hash for.
     * @return The hash of the given uninterpreted function.
     */
    std::size_t operator()(const carl::UFContent& ufun) const {
        /*hash<carl::Sort> h;
        std::size_t result = carl::hash_all(ufun.name());
        for (auto& dom: ufun.domain()) {
			       carl::hash_add(result, dom);
        }
        return result;*/
        return carl::hash_all(ufun.name(), ufun.domain());
    }
};
} // end namespace std

namespace carl
{

/**
 * Implements a manager for uninterpreted functions, containing their actual contents and allocating their ids.
 */
class UFManager : public Singleton<UFManager> {
    friend Singleton<UFManager>;

    private:
        /// Stores all instantiated uninterpreted function's contents and maps them to their unique id.
        FastPointerMap<UFContent, UninterpretedFunction::IDType> mUFIdMap;
        /// Maps the unique ids to the instantiated uninterpreted function's content.
        std::vector<std::unique_ptr<UFContent>> mUFs;

        /**
         * Constructs an uninterpreted functions manager.
         */
        UFManager() {
            mUFs.emplace_back(nullptr); // default value
        }
        ~UFManager() override {
            mUFIdMap.clear();
            mUFs.clear();
        }

        /**
         * Tries to add the given uninterpreted function's content to the so far stored uninterpreted function's
         * contents. If it has already been stored, if will be deleted and the id of the already existing uninterpreted
         * function's content will be used to create the returned uninterpreted function.
         * @param sc The uninterpreted function's content to store.
         * @return The uninterpreted function corresponding to the given content.
         */
        UninterpretedFunction newUF(std::unique_ptr<UFContent>&& sc);

    public:
        const std::vector<std::unique_ptr<UFContent>>& ufContents() const {
            return mUFs;
        }
        const FastPointerMap<UFContent, UninterpretedFunction::IDType>& ufIDMap() const {
            return mUFIdMap;
        }

        /**
         * @param uf An uninterpreted function.
         * @return The name of the uninterpreted function of the given uninterpreted function.
         */
        const std::string& getName(const UninterpretedFunction& uf) const {
            assert(uf.id() != 0);
            assert(uf.id() < mUFs.size());
            return mUFs[uf.id()]->name();
        }

        /**
         * @param uf An uninterpreted function.
         * @return The domain of the uninterpreted function of the given uninterpreted function.
         */
        const std::vector<Sort>& getDomain(const UninterpretedFunction& uf) const {
            assert(uf.id() != 0);
            assert(uf.id() < mUFs.size());
            return mUFs[uf.id()]->domain();
        }

        /**
         * @param uf An uninterpreted function.
         * @return The codomain of the uninterpreted function of the given uninterpreted function.
         */
        const Sort& getCodomain(const UninterpretedFunction& uf) const {
            assert(uf.id() != 0);
            assert(uf.id() < mUFs.size());
            return mUFs[uf.id()]->codomain();
        }

        /**
         * Gets the uninterpreted function with the given name, domain, arguments and codomain.
         * @param name The name of the uninterpreted function of the uninterpreted function to get.
         * @param domain The domain of the uninterpreted function of the uninterpreted function to get.
         * @param codomain The codomain of the uninterpreted function of the uninterpreted function to get.
         * @return The resulting uninterpreted function.
         */
        UninterpretedFunction newUninterpretedFunction(std::string&& name, std::vector<Sort>&& domain, Sort codomain) {
            return newUF(std::make_unique<UFContent>(std::move(name), std::move(domain), codomain));
        }
};

/**
 * Gets the uninterpreted function with the given name, domain, arguments and codomain.
 * @param name The name of the uninterpreted function of the uninterpreted function to get.
 * @param domain The domain of the uninterpreted function of the uninterpreted function to get.
 * @param codomain The codomain of the uninterpreted function of the uninterpreted function to get.
 * @return The resulting uninterpreted function.
 */
inline UninterpretedFunction newUninterpretedFunction(std::string name, std::vector<Sort> domain, Sort codomain) {
    return UFManager::getInstance().newUninterpretedFunction(std::move(name), std::move(domain), codomain);
}

}
