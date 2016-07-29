

%module ModRational 
%{

#include "../numbers/numbers.h"
#include "gmp.h"
#include "gmpxx.h"

typedef mpq_class Rational;
%}




%include "std_string.i"


class Rational {
//NOTE: maybe change this to getDouble or whatever..
%rename(toDouble) get_d;
public:
Rational(signed int i) { mpq_init(mp); mpq_set_si(mp, i, 1); }
Rational(double d) { mpq_init(mp); mpq_set_d(mp, d); }
  explicit Rational(const std::string &s, int base = 0)
  {
    mpq_init(mp);
    if (mpq_set_str (mp, s.c_str(), base) != 0)
      {
        mpq_clear (mp);
        throw std::invalid_argument ("mpq_set_str");
      }
  }

 double get_d() const;

%extend{
 std::string toString() {
   return carl::toString(*$self,true);	
 } 
}
//TODO: what else is needed?


/*


//TODO: Operators!!
carl::sint toInt() {
    double d = carl::toDouble(*$self);
    return static_cast<carl::sint>(d);
}

double toDouble() {
 return carl::toDouble(*$self);
}
   
std::string toString() {
    std::stringstream ss;
    ss << *$self;
    return ss.str();
}

int nominator() {
 return carl::toInt<carl::sint>(carl::getNum(*$self));
}

int numerator() {
 return carl::toInt<carl::sint>(carl::getNum(*$self));
}

int denominator() {
 return carl::toInt<carl::sint>(carl::getDenom(*$self));
}

//TODO: maybe setstate/getstate..
*/

};



//%rename(Rational) mpq_class;

typedef mpq_class Rational;
