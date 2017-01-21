/* 
 * File:   PolynomialSorts.h
 * Author: Sebastian Junges
 *
 */

#pragma once

/**
 * Sorts generators of an ideal by their leading terms.
 * @param generators
 */
template<class Polynomial>
class sortByLeadingTerm
{
public:

    explicit sortByLeadingTerm(const std::vector<Polynomial>& generators): mGenerators(generators) {
    }

    bool operator()(std::size_t a, std::size_t b) const
    {
		assert(a < mGenerators.size()); 
		assert(b < mGenerators.size());
        return Polynomial::compareByLeadingTerm(mGenerators[a], mGenerators[b]);
    }
private:
    const std::vector<Polynomial>& mGenerators;
};

/**
 * Sorts generators of an ideal by their number of terms.
 * @param generators
 */
template<class Polynomial>
class sortByPolSize
{
public:

    explicit sortByPolSize(const std::vector<Polynomial>& generators): mGenerators(generators) {
    }

    bool operator()(std::size_t a, std::size_t b) const {
        return Polynomial::compareByNrTerms(mGenerators[a], mGenerators[b]);
    }
private:
    const std::vector<Polynomial>& mGenerators;
};
