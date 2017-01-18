/* 
 * File:   MultiplicationTable2.h
 * Author: tobias
 *
 * Created on 8. August 2016, 14:32
 */

#pragma once

#include "GroebnerBase.h"

namespace carl {
	
	
/*
 * these objects represent elements from a certain vector space as a linear combinations of its base.
 * rather than storing the elements in the base we only store indices.
 */
template<typename Number>
struct BaseRepresentation : public std::map<uint, Number> {
	using Monomial = Term<Number>;
	
	BaseRepresentation() = default;
	
	BaseRepresentation(const std::vector<Monomial>& base, const MultivariatePolynomial<Number>& p) {
		CARL_LOG_ASSERT("carl.thom.tarski", base.size() >= p.size(), "p is not in <base>");
		for(const auto& term : p) {
			for(uint i = 0; i < base.size(); i++) {
				if(term.monomial() == base[i].monomial()) {
					this->insert(std::make_pair(i, term.coeff()));
				}
			}	  
		}
	}
		
	bool isZero() const { return this->empty(); }
	bool contains(uint i) const { return this->find(i) != this->end(); }
	Number get(uint index) const {
		auto it = this->find(index);
		if(it == this->end()) return Number(0);
		else return it->second;
	}
};

/*
 * Let Mon be the base of the structure Q[x_1,...x_n] / <gb> viewed as a Q vector space
 * this class stores for each monomial m in {ab | a,b in Mon } the following entry:
 * 
 * Monomial     |       BaseRepresentation      |       Index pairs
 * 
 * where the base representation is the monomial viewed as a linear combination of the basis
 * and index pairs stores a list of all pairs of basis elements whose product is equal to Monomial
 */
template<typename Number>
class MultiplicationTable {
	
public:
	
	using IndexPairs = std::forward_list<std::pair<uint, uint>>;
	using Monomial = Term<Number>;
	
	struct TableContent {
		BaseRepresentation<Number> br;
		IndexPairs pairs;
	};
private:

	// main datastructure
	std::unordered_map<Monomial, TableContent> mTable;
	
	// the base of the factor ring as a vector space
	std::vector<Monomial> mBase;
	
	// the groebner base object is used to compute reductions
	GroebnerBase<Number> mGb;
	
public:
	
	MultiplicationTable() : mTable(), mBase(), mGb() {}
	
	explicit MultiplicationTable(const GroebnerBase<Number>& gb) : mGb(gb){
		CARL_LOG_ASSERT("carl.thom.tarski.table", gb.hasFiniteMon(), "tried to set up a multiplication table on infinite basis");
		init(gb);
		CARL_LOG_TRACE("carl.thom.tarski.table", "done setting up multiplication table:\n" << *this);
	}
	
	typename std::unordered_map<Monomial, TableContent>::const_iterator begin() const { return mTable.cbegin(); }
	typename std::unordered_map<Monomial, TableContent>::const_iterator end() const { return mTable.cend(); }
	typename std::unordered_map<Monomial, TableContent>::const_iterator cbegin() const { return mTable.cbegin(); }
	typename std::unordered_map<Monomial, TableContent>::const_iterator cend() const { return mTable.cend(); }
	
	
	inline bool contains(const Monomial& m) const {
		return mTable.find(m) != mTable.end();
	}
	
	const std::vector<Monomial>& getBase() const noexcept {
		return mBase;
	}
	
	BaseRepresentation<Number> reduce(const MultivariatePolynomial<Number>& p) const {
		return BaseRepresentation<Number>(mBase, mGb.reduce(p));
	}
	
	const TableContent& getEntry(const Monomial& mon) const {
		auto it = mTable.find(mon);
		assert(it != mTable.end());
		return it->second;
	}
	
	MultivariatePolynomial<Number> baseReprToPolynomial(const BaseRepresentation<Number>& baseRepr) const {
		MultivariatePolynomial<Number> res(Number(0));
		for(const auto& entry : baseRepr) {
			res += entry.second * this->mBase[entry.first];
		}
		return res;
	}
	
	BaseRepresentation<Number> multiply(const BaseRepresentation<Number>& f, const BaseRepresentation<Number>& g) const {
		BaseRepresentation<Number> res;
		for(const auto& entry : this->mTable) {
			if(entry.second.br.isZero()) continue;
			for(const auto& index_coeff : entry.second.br) {
				Number newCoeff(0);
				for(const auto& pair : entry.second.pairs) {
					newCoeff += f.get(pair.first) * g.get(pair.second);
				}
				if(newCoeff != 0) {
					res[index_coeff.first] += newCoeff * index_coeff.second;
				}
			}
		}
		return res;
	}
	
	
	Number trace(const BaseRepresentation<Number>& f) const {
		Number res(0);
		for(const auto& index_coeff : f) {
			for(uint i = 0; i < mBase.size(); i++) {
				Monomial prod = mBase[index_coeff.first] * mBase[i];
				BaseRepresentation<Number> prod_br = this->getEntry(prod).br;
				res += index_coeff.second * prod_br.get(i);
			}		   
		}
		return res;
	}
	
	template<typename C>
	friend std::ostream& operator<<(std::ostream& o, const MultiplicationTable<C>& table);
	
private:
	
	// returns a list of all pairs of indicdes (i,j) such that base_i * base_j == c
	IndexPairs indexPairs(const Monomial& c) const {
		IndexPairs res;
		for(uint i = 0; i < mBase.size(); i++) {
			for(uint j = i; j < mBase.size(); j++) {
				if(mBase[i] * mBase[j] == c) {
					res.push_front(std::make_pair(i, j));
					if(i != j) res.push_front(std::make_pair(j, i));
				}
			}
		}
		return res;
	}
	
	void init(const GroebnerBase<Number>& gb) {
		CARL_LOG_FUNC("carl.thom.tarski", "gb = " << gb.get());
		
		// some needed values
		std::vector<Monomial> Cor = gb.cor();
		std::vector<Monomial> Bor = gb.bor();
		std::vector<Monomial> Mon = gb.mon();
		std::set<Variable> vars = gb.gatherVariables();
		
		// sort the base
		std::sort(Mon.begin(), Mon.end());
		mBase = Mon;
		
		// monomials in bor in increasing order
		std::sort(Bor.begin(), Bor.end());
		
		// ---- step 0 ---- (not explicitly mentioned)
		// put BaseReprensentation of the monomials from Mon itself in table
		for(uint i = 0; i < Mon.size(); i++) {
			BaseRepresentation<Number> baseRepr;
			baseRepr[i] = Number(1); 
			mTable[Mon[i]] = {baseRepr, indexPairs(Mon[i])};
		}
		
		// ---- step 1 ----
		// compute the normal forms of the elements in Bor
		for(const auto& m : Bor) {
			if(std::find(Cor.begin(), Cor.end(), m) != Cor.end()) {
				CARL_LOG_TRACE("carl.thom.tarski.table", "in bor and also in cor: " << m);
				// the monomial is also in Cor
				// find the polynomial in the basis with the corresponding leading monomial
				MultivariatePolynomial<Number> G;
				for(const auto& p : gb.get()) {
					assert(!p.isZero());
					if(p.lterm() == m) {
						G = p;
						break;
					}
				}
				MultivariatePolynomial<Number> diff = G.stripLT();
				diff *= Number(-1);
				BaseRepresentation<Number> baseRepr(mBase, diff);
				IndexPairs pairs;
				if(!baseRepr.isZero()) {
					pairs = indexPairs(m);
				}
				mTable[m] = {baseRepr, pairs};
				//CARL_LOG_TRACE("carl.thom.tarski", "mTable = " << mTable);			     
			}
			else {
				// try to find a variable X_j, such that m / X_j is in Bor
				CARL_LOG_TRACE("carl.thom.tarski.table", "not in cor but in bor: " << m);
				auto it = vars.begin();
				Variable var = *it;
				Monomial x_beta;
				while(!m.divide(var, x_beta) || (std::find(Bor.begin(), Bor.end(), x_beta) == Bor.end())) {				    
					it++;
					CARL_LOG_ASSERT("carl.thom.tarski.table", it != vars.end(), "");
					var = *it;
				}
				CARL_LOG_ASSERT("carl.thom.tarski.table", std::find(Bor.begin(), Bor.end(), x_beta) != Bor.end(), "");
				CARL_LOG_TRACE("carl.thom.tarski.table", "x_beta = " << x_beta);
				CARL_LOG_TRACE("carl.thom.tarski.table", "var = " << var);
				CARL_LOG_ASSERT("carl.thom.tarski.table", this->contains(x_beta), "");
				CARL_LOG_ASSERT("carl.thom.tarski.table", x_beta < m, "");
				
				BaseRepresentation<Number> nf_x_beta = mTable[x_beta].br;
				MultivariatePolynomial<Number> sum(Number(0));
				
				// sum over all pairs in Mon^2...
				CARL_LOG_TRACE("carl.thom.tarski.table", "nf_x_beta" << nf_x_beta);
				for(const auto& entry_beta : nf_x_beta) {
					Monomial x_gamma_prime = var * Mon[entry_beta.first];
					CARL_LOG_ASSERT("carl.thom.tarski.table", x_gamma_prime < m, "");
					CARL_LOG_ASSERT("carl.thom.tarski.table", this->contains(x_gamma_prime), "");
					BaseRepresentation<Number> nf_x_gamma_prime = mTable[x_gamma_prime].br;
					CARL_LOG_TRACE("carl.thom.tarski.table", "nf_x_gamma_prime" << nf_x_gamma_prime);
					for(const auto& entry_gamma : nf_x_gamma_prime) {
						Term<Number> prod(nf_x_beta[entry_beta.first]);
						prod = prod * nf_x_gamma_prime[entry_gamma.first];
						prod = prod * Mon[entry_gamma.first];
						sum += prod;
					}
				}
				IndexPairs pairs;
				BaseRepresentation<Number> baseRepr(mBase, sum);
				if(!baseRepr.isZero()) {
					pairs = indexPairs(m);
				}
				mTable[m] = {baseRepr, pairs};
			}
		}
		
		// ---- step 2 ----
		// construct the matrices expressing multiplication by a variable (see step 2)
		
		// ---- step 3 ----
		// find the normal forms of all other elements in Tab(Mon)
		// Tab(Mon) = set of products of elements from Mon
		std::list<Monomial> tabmon;
		for(const auto& m1 : Mon) {
			for(const auto& m2 : Mon) {
				Monomial prod = m1 * m2;
				if(std::find(tabmon.begin(), tabmon.end(), prod) == tabmon.end()) {
					tabmon.push_back(prod);
				}				
			}
		}
		CARL_LOG_TRACE("carl.thom.tarski", "tabmon = " << tabmon);
		for(const auto& m : tabmon) {
			if(!this->contains(m)) {
				// we do not have the normal form of m yet
				CARL_LOG_TRACE("carl.thom.tarski.table", "still to compute: normal form for " << m);
				
				// make it easy here and use groebner reduce
				MultivariatePolynomial<Number> nf_m = mGb.reduce(MultivariatePolynomial<Number>(m));
				
				IndexPairs pairs;
				BaseRepresentation<Number> baseRepr(mBase, nf_m);
				if(!baseRepr.isZero()) {
					pairs = indexPairs(m);
				}
				mTable[m] = {baseRepr, pairs};
			}
		}
	}
};

template<typename C>
std::ostream& operator<<(std::ostream& o, const MultiplicationTable<C>& table) {
	o << "Base = " << table.mBase << std::endl;
	o << "Length = " << table.mTable.size() << std::endl;
	for (const auto& entry: table.mTable) {
		o << entry.first << "\t" << entry.second.br << "\t" << entry.second.pairs << std::endl;
	}
	return o;
}

}
