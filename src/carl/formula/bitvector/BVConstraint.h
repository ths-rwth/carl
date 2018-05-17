/**
 * @file BVConstraint.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVCompareRelation.h"
#include "BVTerm.h"

namespace carl
{

	// Forward declaration
	class BVConstraintPool;

	class BVConstraint
	{
		friend class BVConstraintPool;

	private:
		/// The hash value.
		std::size_t mHash = 0;
		/// The unique id.
		std::size_t mId = 0;

		/// The relation for comparing left- and right-hand side.
		BVCompareRelation mRelation;
		/// The left-hand side of the (in)equality.
		BVTerm mLhs;
		/// The right-hand size of the (in)equality.
		BVTerm mRhs;


		explicit BVConstraint(bool _consistent = true) :
			mRelation(_consistent ? BVCompareRelation::EQ : BVCompareRelation::NEQ)
		{}

		/**
		 * Constructs the constraint: _lhs _relation _rhs
		 * @param _lhs The bit-vector term to be used as left-hand side of the constraint.
		 * @param _rhs The bit-vector term to be used as right-hand side of the constraint.
		 * @param _relation The relation symbol to be used for the constraint.
		 */
		BVConstraint(const BVCompareRelation& _relation,
			const BVTerm& _lhs, const BVTerm& _rhs) :
			mHash((toId(_relation) << 10) ^ (_lhs.hash() << 5) ^ _rhs.hash()),
			mRelation(_relation), mLhs(_lhs), mRhs(_rhs)
		{
			assert(_lhs.width() == _rhs.width());
		}

	public:

		static BVConstraint create(bool _consistent = true);

		static BVConstraint create(const BVCompareRelation& _relation,
			const BVTerm& _lhs, const BVTerm& _rhs);

		/**
		 * @return The bit-vector term being the left-hand side of this constraint.
		 */
		const BVTerm& lhs() const
		{
			return mLhs;
		}

		/**
		 * @return The bit-vector term being the right-hand side of this constraint.
		 */
		const BVTerm& rhs() const
		{
			return mRhs;
		}

		/**
		 * @return The relation symbol of this constraint.
		 */
		BVCompareRelation relation() const
		{
			return mRelation;
		}

		/**
		 * @return The unique id of this constraint.
		 */
		std::size_t id() const
		{
			return mId;
		}
		
		void collectVariables(std::set<BVVariable>& vars) const {
			mLhs.collectVariables(vars);
			mRhs.collectVariables(vars);
		}

		/**
		 * @return A hash value for this constraint.
		 */
		std::size_t getHash() const
		{
			return mHash;
		}

		bool isConstant() const
		{
            return mLhs.isInvalid() && mRhs.isInvalid();
        }

		bool isAlwaysConsistent() const
		{
            return isConstant() && mRelation == BVCompareRelation::EQ;
        }

        bool isAlwaysInconsistent() const
        {
            return isConstant() && mRelation == BVCompareRelation::NEQ;
        }

		/**
		 * Gives the string representation of this bit vector constraint.
		 * @param _init The initial string of every row of the result.
		 * @param _oneline A flag indicating whether the constraint shall be printed on one line.
		 * @param _infix A flag indicating whether to print the constraint in infix or prefix notation.
		 * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
		 *                        or with their dedicated names.
		 * @return The resulting string representation of this constraint.
		 */
		std::string toString(const std::string& _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const
		{
            if(isAlwaysConsistent()) {
                return _init + "TRUE";
            } else if(isAlwaysInconsistent()) {
                return _init + "FALSE";
            } else {
                std::string out = _init + "(";

                if(!_infix)
                    out += carl::toString(mRelation) + (_oneline ? " " : "");

                if(!_oneline)
                    out += "\n";

                out += mLhs.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
                out += (_oneline ? " " : "\n");

                if(_infix)
                    out += _init + carl::toString(mRelation) + (_oneline ? " " : "\n");

                out += mRhs.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
                out += ")";

                return out;
            }
		}

		/**
		 * The output operator of a constraint.
		 * @param _out The stream to print on.
		 * @param _constraint The constraint to be printed.
		 */
		friend std::ostream& operator<<(std::ostream& _out, const BVConstraint& _constraint)
		{
			return(_out << _constraint.toString());
		}

		bool operator==(const BVConstraint& _other) const
		{
			if(mId != 0 && _other.mId != 0) {
				return mId == _other.mId;
			}

			return
				mRelation == _other.mRelation
				&& mLhs == _other.mLhs
				&& mRhs == _other.mRhs;
		}
		bool operator<(const BVConstraint& _other) const
		{
			if (mId != 0 && _other.mId != 0) {
				return mId < _other.mId;
			}
			if (mRelation != _other.mRelation) return mRelation < _other.mRelation;
			if (!(mLhs == _other.mLhs)) return mLhs < _other.mLhs;
			if (!(mRhs == _other.mRhs)) return mRhs < _other.mRhs;
			return false;
		}

		std::size_t hash() const
		{
			return mHash;
		}
            
        /**
         * @return An approximation of the complexity of this bit vector constraint.
         */
        size_t complexity() const
        {
            return 1 + mLhs.complexity() + mRhs.complexity();
        }
	};
} // namespace carl


namespace std
{
	/**
	 * Implements std::hash for bit-vector constraints.
	 */
	template <>
	struct hash<carl::BVConstraint>
	{
		public:

		/**
		 * @param _constraint The bit-vector constraint to get the hash for.
		 * @return The hash of the given constraint.
		 */
		size_t operator()(const carl::BVConstraint& _constraint) const
		{
			return _constraint.hash();
		}
	};
} // namespace std
