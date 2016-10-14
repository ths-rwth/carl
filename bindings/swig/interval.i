



%module IntervalRationalT
%{
#include <carl/interval/Interval.h>

typedef mpq_class Rational;
typedef unsigned int uint;
%}

%import "rational.i"

typedef unsigned int uint;


namespace carl {


%include "enums.swg"
%javaconst(1);


enum class BoundType {
        /// the given bound is compared by a strict ordering relation
        STRICT=0,
        /// the given bound is compared by a weak ordering relation
        WEAK=1,
        /// the given bound is interpreted as minus or plus infinity depending on whether it is the left or the right bound
        INFTY=2
    };



template<typename Number>
class Interval
{
public:

Interval(const Number& n) :
mContent(n),
mLowerBoundType(BoundType::WEAK),
mUpperBoundType(BoundType::WEAK) { }

Interval(const Number& lower, const Number& upper)
{

}

Interval(const Number& lower, BoundType lowerBoundType, const Number& upper, BoundType upperBoundType)
{
}


//BEWARE: in python sth with return value policy had to be changed here, so maybe problems will arise

inline const Number& lower() const;

inline const Number& upper() const;

inline void setLower(const Number& n);

inline void setUpper(const Number& n);

inline bool isInfinite() const;

inline bool isUnbounded() const;

inline bool isHalfBounded() const;

inline bool isEmpty() const;

inline bool isPointInterval() const;

inline bool isOpenInterval() const;

inline bool isClosedInterval() const;

inline bool isZero() const;

inline bool isOne() const;

inline bool isPositive() const;

inline bool isNegative() const;

inline bool isSemiPositive() const;

inline bool isSemiNegative() const;

Interval<Number> integralPart() const;
Number diameter() const;
//NOTE: this is now set for Number not being a float... needs to be changed in Interval.h otherwise
Number center() const;
Number sample(bool _includingBounds = true) const;
bool contains(const Number& val) const;
bool contains(const Interval<Number>& rhs) const;
bool meets(const Number& val) const;
bool isSubset(const Interval<Number>& rhs) const;
bool isProperSubset(const Interval<Number>& rhs) const;

Interval<Number> div(const Interval<Number>& rhs) const;
Interval<Number> inverse() const;
Interval<Number> abs() const;
Interval<Number> pow(uint exp) const;
bool intersectsWith(const Interval<Number>& rhs) const;

Interval<Number> intersect(const Interval<Number>& rhs) const;
bool unite(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const;
bool difference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const;
bool complement(Interval<Number>& resultA, Interval<Number>& resultB) const;
bool symmetricDifference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const;




std::string toString() const;

%extend {
/*
static Interval<Number> pow(const Interval<Number>& _in, unsigned _exponent)
{
        return carl::pow(_in,_exponent);
} */

static Interval<Number> floor(const Interval<Number>& _in) {
 return carl::floor(_in);
}

static Interval<Number> ceil(const Interval<Number>& _in) {
 return carl::ceil(_in);
}

/*
static Interval<Number> abs(const Interval<Number>& _in) {
 return carl::abs(_in);
}

static Interval<Number> div(const Interval<Number>& _lhs, const Interval<Number>& _rhs) {
 return carl::div(_lhs,_rhs);
} */

static Interval<Number> quotient(const Interval<Number>& _lhs, const Interval<Number>& _rhs) {
 return carl::div(_lhs,_rhs);
}

static bool isInteger(const Interval<Number>&) {
 return false;
}




Interval<Number> add(const Interval<Number>& rhs) {
	return *$self+rhs;
}

Interval<Number> add(const Rational& rhs) {
	return *$self+rhs;
}

Interval<Number> neg() {
	return -*$self;
}

Interval<Number> sub(const Interval<Number>& rhs) {
	return *$self-rhs;
}

Interval<Number> sub(const Rational& rhs) {
	return *$self-rhs;
}

Interval<Number> mul(const Interval<Number>& rhs) {
	return *$self*rhs;
}

Interval<Number> mul(const Rational& rhs) {
	return *$self*rhs;
}




Interval<Number> div(const Rational& rhs) {
	return *$self+rhs;
}


//The +=, -=, *= are missing now



bool equals(const Interval<Number> rhs) {
	return *$self==rhs;
}

bool notEquals(const Interval<Number> rhs) {
	return *$self!=rhs;
}

bool lessEquals(const Interval<Number> rhs) {
	return *$self<=rhs;
}

bool lessEquals(const Rational rhs) {
	return *$self<=rhs;
}

bool greaterEquals(const Interval<Number> rhs) {
	return *$self>=rhs;
}

bool greaterEquals(const Rational rhs) {
	return *$self>=rhs;
}


bool less(const Interval<Number> rhs) {
	return *$self<rhs;
}

bool less(const Rational rhs) {
	return *$self<rhs;
}

bool greater(const Interval<Number> rhs) {
	return *$self>rhs;
}

bool greater(const Rational rhs) {
	return *$self>rhs;
}



}

};

}
%template(IntervalRational) carl::Interval<Rational>;

