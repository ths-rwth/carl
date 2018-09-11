/**
 * @file UEquality.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-20
 * @version 2014-10-22
 */

#include "UEquality.h"

namespace carl
{
	std::size_t UEquality::complexity() const {
		return 1 + mLhs.complexity() + mRhs.complexity();
	}

    bool UEquality::operator==(const UEquality& ueq) const {
        if(mNegated != ueq.negated()) {
            return false;
        }
        if(mLhs.isUVariable() != ueq.lhs().isUVariable()) {
            return false;
        }
        if(mRhs.isUVariable() != ueq.rhs().isUVariable()) {
            return false;
        }
        if(mLhs.isUVariable()) {
            if(mRhs.isUVariable()) {
                return mLhs.asUVariable() == ueq.lhs().asUVariable() && mRhs.asUVariable() == ueq.rhs().asUVariable();
            } else {
                return mLhs.asUVariable() == ueq.lhs().asUVariable() && mRhs.asUFInstance() == ueq.rhs().asUFInstance();
            }
        } else {
            if(mRhs.isUVariable()) {
                return mLhs.asUFInstance() == ueq.lhs().asUFInstance() && mRhs.asUVariable() == ueq.rhs().asUVariable();
            } else {
                return mLhs.asUFInstance() == ueq.lhs().asUFInstance() && mRhs.asUFInstance() == ueq.rhs().asUFInstance();
            }
        }
    }

    bool UEquality::operator<(const UEquality& ueq) const {
        if(!mNegated && ueq.negated()) {
            return true;
        }
        if(mNegated && !ueq.negated()) {
            return false;
        }
        if(mLhs.isUVariable() && !ueq.lhs().isUVariable()) {
            return true;
        } else if(!mLhs.isUVariable() && ueq.lhs().isUVariable()) {
            return false;
        }
        if(mRhs.isUVariable() && !ueq.rhs().isUVariable()) {
            return true;
        } else if(!mRhs.isUVariable() && ueq.rhs().isUVariable()) {
            return false;
        }
        assert(mLhs.isUVariable() == ueq.lhs().isUVariable());
        if(mLhs.isUVariable()) {
            if(mLhs.asUVariable() < ueq.lhs().asUVariable()) {
                return true;
            } else if(ueq.lhs().asUVariable() < mLhs.asUVariable()) {
                return false;
            }
        } else {
            if(mLhs.asUFInstance() < ueq.lhs().asUFInstance()) {
                return true;
            } else if(ueq.lhs().asUFInstance() < mLhs.asUFInstance()) {
                return false;
            }
        }
        assert((mLhs.isUVariable() && mLhs.asUVariable() == ueq.lhs().asUVariable()) || (mLhs.isUFInstance() && mLhs.asUFInstance() == ueq.lhs().asUFInstance()));
        if(mRhs.isUVariable()) {
            return mRhs.asUVariable() < ueq.rhs().asUVariable();
        } else {
            return mRhs.asUFInstance() < ueq.rhs().asUFInstance();
        }
    }

    std::string UEquality::toString(unsigned unequalSwitch, bool infix, bool friendlyNames) const {
        std::string result = "";
        if(!infix) {
            if(negated()) {
                if(unequalSwitch == 0) {
                  result += "(<> ";
                } else if (unequalSwitch == 1) {
                  result += "(not (= ";
                } else {
                  result += "(!= ";
                }
            } else {
                result += "(= ";
            }
        }
        if(mLhs.isUVariable()) {
            result += mLhs.asUVariable().toString(friendlyNames);
        } else {
            result += mLhs.asUFInstance().toString(infix, friendlyNames);
        }
        if(infix) {
            result += (negated() ? "!=" : "=");
        } else {
            result += " ";
        }
        if(mRhs.isUVariable()) {
            result += mRhs.asUVariable().toString(friendlyNames);
        } else {
            result += mRhs.asUFInstance().toString(infix, friendlyNames);
        }
        if(!infix) {
            if (negated()) {
                if (unequalSwitch == 1) {
                  result += ")";
                }
            }
            result += ")";
        }
        return result;
    }

    void collectUFVars(std::set<UVariable>& uvars, UFInstance ufi) {
      for(const auto& arg : ufi.args()) {
          if(arg.isUVariable()) {
              uvars.insert(arg.asUVariable());
          } else if(arg.isUFInstance()) {
              collectUFVars(uvars, arg.asUFInstance());
          }
      }
    }

    void UEquality::collectUVariables(std::set<UVariable>& uvars) const {
        if(mLhs.isUVariable()) {
            uvars.insert(mLhs.asUVariable());
        } else {
            collectUFVars(uvars, mLhs.asUFInstance());
        }
        if(mRhs.isUVariable()) {
            uvars.insert(mRhs.asUVariable());
        } else {
            collectUFVars(uvars, mRhs.asUFInstance());
        }
    }

	std::ostream& operator<<(std::ostream& os, const UEquality& ueq) {
		return os << ueq.lhs() << (ueq.negated() ? " != " : " == ") << ueq.rhs();
	}
} // end namespace carl
