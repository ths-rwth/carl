
%module ModVariable
%{
#include <carl/core/Variable.h>
#include <carl/core/Monomial.h>
%}



%include "std_string.i"



//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least


namespace carl {


%include "enums.swg"
%javaconst(1);
//maybe also define the toString for this enum, if needed
enum VariableType { VT_BOOL = 0, VT_REAL = 1, VT_INT = 2, VT_UNINTERPRETED = 3, VT_BITVECTOR = 4, MIN_TYPE = VT_BOOL, MAX_TYPE = VT_BITVECTOR};




class Variable {
public:
#ifdef VARIABLE_PASS_BY_VALUE
	/// Argument type for variables being function arguments.
	typedef Variable Arg;
#else
	/// Argument type for variables being function arguments.
	typedef const Variable& Arg;
#endif
	%extend {
		 Variable(std::string name, carl::VariableType type) {
		   //try and generate new variable from pool
		     carl::Variable newVar;
	 	     auto& pool = carl::VariablePool::getInstance();
    		     carl::Variable res = pool.findVariableWithName(name);
		     if (res != carl::Variable::NO_VARIABLE) {
			 newVar = res;
		     } else {
		     	 newVar = freshVariable(name, type);
		     }	
		     return new carl::Variable(newVar);		
		}

		Variable(carl::VariableType type)  {
	       		 carl::Variable newVar = freshVariable(type);
			 return new carl::Variable(newVar);
	    	}

		
		std::string toString() {
		     std::stringstream ss;
		     ss << *$self;
		     return ss.str();	
		}

	}
	
std::size_t getId() const; 


VariableType getType() const;


std::string getName() const;


std::size_t getRank();

%extend {
	bool equals(carl::Variable::Arg other) {
		return *$self == other;			
	}

	bool notEquals(carl::Variable::Arg other) {
		return *$self != other;			
	}
	bool less(carl::Variable::Arg other) {
		return *$self < other;			
	}
	bool lessEquals(carl::Variable::Arg other) {
		return *$self <= other;			
	}
	bool greater(carl::Variable::Arg other) {
		return *$self > other;			
	}
	bool greaterEquals(carl::Variable::Arg other) {
		return *$self >= other;			
	}
}	
};

}

