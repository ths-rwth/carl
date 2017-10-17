/** 
 * @file:   RationalFunction.h
 * @author: Sebastian Junges
 * @author: Florian Corzilius
 */

#pragma once
#include "../numbers/numbers.h"
#include "MultivariateGCD.h"
#include "FactorizedPolynomial.h"

namespace carl
{
	
    template<typename Pol, bool AutoSimplify=false>
    class RationalFunction
    {
    public:
        
        using PolyType = Pol;
        using CoeffType = typename Pol::CoeffType;
		using NumberType = typename Pol::NumberType;
        
    private:
        
        std::pair<Pol,Pol>* mPolynomialQuotient;
        CoeffType mNumberQuotient;
        bool mIsSimplified;

    public:
        
        RationalFunction(): 
            mPolynomialQuotient(nullptr),
            mNumberQuotient(0),
            mIsSimplified(true)
        {}

        explicit RationalFunction(int v): 
            mPolynomialQuotient(nullptr),
            mNumberQuotient(v), 
            mIsSimplified(true)
        {}

        explicit RationalFunction(const CoeffType& c): 
            mPolynomialQuotient(nullptr),
            mNumberQuotient(c), 
            mIsSimplified(true)
        {}

        template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
        explicit RationalFunction(Variable::Arg v): 
            mPolynomialQuotient(new std::pair<Pol,Pol>(std::move(P(v)), std::move(Pol(1)))), 
            mNumberQuotient(),
            mIsSimplified(true)
        {}

        explicit RationalFunction(const Pol& p):
            mPolynomialQuotient(nullptr),
            mNumberQuotient(),
            mIsSimplified(true)
        {
            if( p.isConstant() )
                mNumberQuotient = p.constantPart();
            else
                mPolynomialQuotient = new std::pair<Pol,Pol>(p, std::move(Pol(1)));
        }

        explicit RationalFunction(Pol&& p):
            mPolynomialQuotient(nullptr),
            mNumberQuotient(),
            mIsSimplified(true)
        {
            if( p.isConstant() )
                mNumberQuotient = p.constantPart();
            else
                mPolynomialQuotient = new std::pair<Pol,Pol>(std::move(p), std::move(Pol(1)));
        }

        explicit RationalFunction(const Pol& nom, const Pol& denom):
            mPolynomialQuotient(nullptr),
            mNumberQuotient(),
            mIsSimplified(false)
        {
            if( nom.isConstant() && denom.isConstant() )
            {
                mNumberQuotient = nom.constantPart() / denom.constantPart();
                mIsSimplified = true;
            }
            else
            {
                mPolynomialQuotient = new std::pair<Pol,Pol>(nom, denom);
                eliminateCommonFactor( !AutoSimplify );
                assert(isConstant() || !denominatorAsPolynomial().isZero());
            }
        }

        explicit RationalFunction(Pol&& nom, Pol&& denom):
            mPolynomialQuotient(new std::pair<Pol,Pol>(std::move(nom),std::move(denom))),
            mNumberQuotient(),
            mIsSimplified(false)
        {
            eliminateCommonFactor( !AutoSimplify );
            assert(isConstant() || !denominatorAsPolynomial().isZero());
        }

		explicit RationalFunction(std::pair<Pol,Pol>* quotient, const CoeffType& num, bool simplified):
			mPolynomialQuotient(quotient),
			mNumberQuotient(num),
			mIsSimplified(simplified)
		{}
        
        RationalFunction(const RationalFunction& _rf):
            mPolynomialQuotient( _rf.isConstant() ? nullptr : new std::pair<Pol,Pol>(_rf.nominator(), _rf.denominator()) ),
            mNumberQuotient( _rf.mNumberQuotient ),
            mIsSimplified(_rf.mIsSimplified)
        {}
        
        RationalFunction(RationalFunction&& _rf):
            mPolynomialQuotient( _rf.mPolynomialQuotient ),
            mNumberQuotient( std::move( _rf.mNumberQuotient ) ),
            mIsSimplified(_rf.mIsSimplified)
        {
            _rf.mPolynomialQuotient = nullptr;
        }
        
        ~RationalFunction()
        {
            if( !isConstant() )
            {
                delete mPolynomialQuotient;
            }
        }
        
        RationalFunction& operator=(const RationalFunction& _rf)
        {
            if( this == &_rf ) return *this;
            mIsSimplified = _rf.mIsSimplified;
            mNumberQuotient = _rf.mNumberQuotient;
            if( !isConstant() )
            {
                delete mPolynomialQuotient;
            }
            mPolynomialQuotient = _rf.isConstant() ? nullptr : new std::pair<Pol,Pol>(_rf.nominatorAsPolynomial(), _rf.denominatorAsPolynomial());
            return *this;
        }
        
        RationalFunction& operator=(RationalFunction&& _rf)
        {
            mIsSimplified = _rf.mIsSimplified;
            if( !isConstant() )
            {
                delete mPolynomialQuotient;
            }
            if( _rf.isConstant() )
            {
                mPolynomialQuotient = nullptr;
            }
            else
            {
                mPolynomialQuotient = _rf.mPolynomialQuotient;
                _rf.mPolynomialQuotient = nullptr;
            }
            mNumberQuotient = std::move(_rf.mNumberQuotient);
            return *this;
        }

        /**
         * @return The nominator
         */
        inline Pol nominator() const
        {
            if( isConstant() )
                return Pol( carl::getNum(mNumberQuotient) );
            else
                return mPolynomialQuotient->first;
        }

        /**
         * @return  The denominator
         */
        inline Pol denominator() const
        {
            if( isConstant() )
                return Pol( carl::getDenom(mNumberQuotient) );
            return mPolynomialQuotient->second;
        }

        /**
         * @return The nominator as a number.
         */
        inline const Pol& nominatorAsPolynomial() const
        {
            assert( !isConstant() );
            return mPolynomialQuotient->first;
        }

        /**
         * @return  The denominator as a polynomial.
         */
        inline const Pol& denominatorAsPolynomial() const
        {
            assert( !isConstant() );
            return mPolynomialQuotient->second;
        }

        /**
         * @return The nominator as a number.
         */
        inline CoeffType nominatorAsNumber() const
        {
            assert( isConstant() );
            return carl::getNum(mNumberQuotient);
        }

        /**
         * @return  The denominator as a polynomial.
         */
        inline CoeffType denominatorAsNumber() const
        {
            assert( isConstant() );
            return carl::getDenom(mNumberQuotient);
        }

        /**
         * Checks if this rational function has been simplified since it's last modification.
         * Note that if AutoSimplify is true, this should always return true.
         * @return If this is simplified.
         */
        inline bool isSimplified() const {
            return mIsSimplified;
        }

        inline void simplify() 
        {
            if( AutoSimplify )
            {
                CARL_LOG_WARN("carl.core","Calling simplify on rational function with AutoSimplify");
            }
            eliminateCommonFactor( false );
        }

		/**
		 * Returns the inverse of this rational function.
		 * @return Inverse of this.
		 */
		RationalFunction inverse() const {
			assert(!this->isZero());
			if (isConstant()) {
				return RationalFunction(nullptr, 1/mNumberQuotient, mIsSimplified);
			} else {
				return RationalFunction(new std::pair<Pol,Pol>(mPolynomialQuotient->second, mPolynomialQuotient->first), carl::constant_zero<CoeffType>().get(), mIsSimplified);
			}
		}

        /**
         * Check whether the rational function is zero
         * @return true if it is
         */
        inline bool isZero() const
        {
            if( isConstant() )
                return carl::isZero( mNumberQuotient );
            assert(!denominatorAsPolynomial().isZero());
            return nominatorAsPolynomial().isZero();
        }

        inline bool isOne() const 
        {
            if( isConstant() )
                return carl::isOne( mNumberQuotient );
            assert(!denominatorAsPolynomial().isZero());
            return nominatorAsPolynomial() == denominatorAsPolynomial();
        }

        inline bool isConstant() const 
        {
            return mPolynomialQuotient == nullptr;
        }

        inline CoeffType constantPart() const 
        {
            if( isConstant() )
                return mNumberQuotient;
            return nominatorAsPolynomial().constantPart() / denominatorAsPolynomial().constantPart();
        }

        /**
         * Collect all occurring variables
         * @return All occcurring variables
         */
        inline std::set<Variable> gatherVariables() const {
            std::set<Variable> vars;
            gatherVariables(vars);
            return vars;
        }

        /**
         * Add all occurring variables to the set vars
         * @param vars
         */
        inline void gatherVariables(std::set<Variable>& vars) const
        {
            if( isConstant() )
                return;
            nominatorAsPolynomial().gatherVariables(vars);
            denominatorAsPolynomial().gatherVariables(vars);
        }
        
        /**
         * Evaluate the polynomial at the point described by substitutions.
         * @param substitutions A mapping from variable to constant values.
         * @return The result of the substitution
         */
        CoeffType evaluate(const std::map<Variable, CoeffType>& substitutions) const
        {
            if ( isConstant() )
            {
                return mNumberQuotient;
            }
            else {
                return nominatorAsPolynomial().evaluate(substitutions) / denominatorAsPolynomial().evaluate(substitutions);
            }
        }
		
		RationalFunction substitute(const std::map<Variable, CoeffType>& substitutions) const
		{
			if (isConstant()) return *this;
			else {
				return RationalFunction(nominatorAsPolynomial().substitute(substitutions), denominatorAsPolynomial().substitute(substitutions));
			}
		}

		/**
		 * Derivative of the rational function with respect to variable x
         * @param x the main variable
         * @param nth which derivative one should take
         * @return 
		 * 
		 * @todo Currently only nth = 1 is supported
		 * @todo Curretnly only factorized polynomials are supported
         */
		RationalFunction derivative(const Variable& x, unsigned nth = 1) const;
		
		std::string toString(bool infix = true, bool friendlyNames = true) const;
    private:
        
        /**
         * Helper function for simplify which eliminates the common factor.
         * @param _justNormalize
         */
        void eliminateCommonFactor( bool _justNormalize );
        
        template<bool byInverse = false>
        RationalFunction& add(const RationalFunction& rhs);
        
        template<bool byInverse = false>
        RationalFunction& add(const Pol& rhs);
        
        template<bool byInverse = false, typename P = Pol, DisableIf<needs_cache<P>> = dummy>
        RationalFunction& add(Variable::Arg rhs);
        
        template<bool byInverse = false>
        RationalFunction& add(const CoeffType& rhs);
        
    public:

        /// @name In-place addition operators
        /// @{
        /**
         * Add something to this rational function and return the changed rational function.
         * @param rhs Right hand side.
         * @return Changed rational function.
         */
        inline RationalFunction& operator+=(const RationalFunction& rhs)
        {
            return this->template add<false>( rhs );
        }

        inline RationalFunction& operator+=(const Pol& rhs)
        {
            return this->template add<false>( rhs );
        }

        inline RationalFunction& operator+=(const Term<CoeffType>& rhs)
        {
            auto tmp = Pol(rhs);
            return this->template add<false>( tmp );
        }

        inline RationalFunction& operator+=(const Monomial::Arg& rhs)
        {
            auto tmp = Pol(rhs);
            return this->template add<false>( tmp );
        }

        template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
        inline RationalFunction& operator+=(Variable::Arg rhs)
        {
            return this->template add<false>( rhs );
        }

        inline RationalFunction& operator+=(const CoeffType& rhs)
        {
            return this->template add<false>( rhs );
        }
        /// @}


        /// @name In-place subtraction operators
        /// @{
        /**
         * Subtract something from this rational function and return the changed rational function.
         * @param rhs Right hand side.
         * @return Changed rational function.
         */
        inline RationalFunction& operator-=(const RationalFunction& rhs)
        {
            return this->template add<true>( rhs );
        }

        inline RationalFunction& operator-=(const Pol& rhs)
        {
            return this->template add<true>( rhs );
        }

        inline RationalFunction& operator-=(const Term<CoeffType>& rhs)
        {
            return (*this -= Pol(rhs));
        }

        inline RationalFunction& operator-=(const Monomial::Arg& rhs)
        {
            return (*this -= Pol(rhs));
        }

        template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
        inline RationalFunction& operator-=(Variable::Arg rhs)
        {
            return this->template add<true>( rhs );
        }

        inline RationalFunction& operator-=(const CoeffType& rhs)
        {
            return this->template add<true>( rhs );
        }
        /// @}

        /// @name In-place multiplication operators
        /// @{
        /**
         * Multiply something with this rational function and return the changed rational function.
         * @param rhs Right hand side.
         * @return Changed rational function.
         */
        RationalFunction& operator*=(const RationalFunction& rhs);
        RationalFunction& operator*=(const Pol& rhs);
        RationalFunction& operator*=(const Term<CoeffType>& rhs) {
            return (*this *= Pol(rhs));
        }
        RationalFunction& operator*=(const Monomial::Arg& rhs) {
            return (*this *= Pol(rhs));
        }
        template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
        RationalFunction& operator*=(Variable::Arg rhs);
        RationalFunction& operator*=(const CoeffType& rhs);
        RationalFunction& operator*=(carl::sint rhs);
        /// @}

        /**
         * Divide this rational function by something and return the changed rational function.
         * @param rhs Right hand side.
         * @return Changed rational function.
         */
        RationalFunction& operator/=(const RationalFunction& rhs);
        RationalFunction& operator/=(const Pol& rhs);
        RationalFunction& operator/=(const Term<CoeffType>& rhs) {
            return (*this /= Pol(rhs));
        }
        RationalFunction& operator/=(const Monomial::Arg& rhs) {
            return (*this /= Pol(rhs));
        }
        template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
        RationalFunction& operator/=(Variable::Arg rhs);
        RationalFunction& operator/=(const CoeffType& rhs);
        RationalFunction& operator/=(unsigned long rhs);
        /// @}
        
        template<typename PolA, bool ASA>
        friend bool operator==(const RationalFunction<PolA, ASA>& lhs, const RationalFunction<PolA, ASA>& rhs);
        
        template<typename PolA, bool ASA>
        friend bool operator<(const RationalFunction<PolA, ASA>& lhs, const RationalFunction<PolA, ASA>& rhs);
        
        template<typename PolA, bool ASA>
        friend std::ostream& operator<<(std::ostream& os, const RationalFunction<PolA, ASA>& rhs);
    };

	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) += rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) += rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) += rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) += rhs );
	}

	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) += rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) += rhs );
	}

    template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= typename Pol::CoeffType(-1) );
	}
    
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) -= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) -= rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) -= rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) -= rhs );
	}

	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) -= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) -= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
	}

	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
	}

    template<typename Pol, bool AS>
    inline RationalFunction<Pol, AS> operator*(const typename Pol::CoeffType& lhs, const RationalFunction<Pol, AS>& rhs)
    {
        return std::move( RationalFunction<Pol, AS>(rhs) *= lhs );
    }

    template<typename Pol, bool AS>
    inline RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, carl::sint rhs)
    {
        return std::move( RationalFunction<Pol, AS>(lhs) *= rhs );
    }

    template<typename Pol, bool AS>
    inline RationalFunction<Pol, AS> operator*( carl::sint lhs, const RationalFunction<Pol, AS>& rhs)
    {
        return std::move( RationalFunction<Pol, AS>(rhs) *= lhs );
    }
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}

	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}
	
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, unsigned long rhs)
	{
		return std::move( RationalFunction<Pol, AS>(lhs) /= rhs );
	}
	
	// TODO: Make more efficient :).
	template<typename Pol, bool AS>
	inline RationalFunction<Pol, AS> pow(unsigned exp, const RationalFunction<Pol, AS>& rf)
	{
		static_assert(!std::is_same<Pol,Pol>::value, "Use carl::pow(rf, exp) instead. ");
		RationalFunction<Pol, AS> res = carl::constant_one<RationalFunction<Pol, AS>>().get();
		for(unsigned i = exp; i > 0; --i) {
			res *= rf;
		}
		return res;
	}
	
	template<typename Pol, bool AS>
	inline bool operator!=(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return !(lhs == rhs);
	}
	
}

namespace std
{
	template<typename Pol, bool AS>
	struct hash<carl::RationalFunction<Pol, AS>> 
	{
		std::size_t operator()(const carl::RationalFunction<Pol, AS>& r) const
        {
            if( r.isConstant() )
            {
                std::hash<typename Pol::CoeffType> h;
                return (h(r.nominatorAsNumber()) << 8) ^ (h(r.denominatorAsNumber()) >> 8);
            }
			std::hash<Pol> h;
			return (h(r.nominatorAsPolynomial()) << 8) ^ (h(r.denominatorAsPolynomial()) >> 8);
		}
	};
}

#include "RationalFunction.tpp"
