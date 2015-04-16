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
	template<typename Pol>
	class BVConstraintPool;

	template<typename Pol>
	class BVConstraint
	{
		// TODO: Assigning mId requires Pool<BVConstraint<Pol>> to be a friend,
		// using the private constructors requires BVConstraintPool<Pol> to be
		// a friend. Should we move mId assignment to subclass?
		friend class Pool<BVConstraint<Pol>>;
		friend class BVConstraintPool<Pol>;

	private:
		/// The hash value.
		std::size_t mHash;
		/// The unique id.
		std::size_t mId;

		/// The relation for comparing left- and right-hand side.
		BVCompareRelation mRelation;
		/// The left-hand side of the (in)equality.
		BVTerm<Pol> mLhs;
		/// The right-hand size of the (in)equality.
		BVTerm<Pol> mRhs;


		BVConstraint(bool _consistent = true) :
		mHash(0), mId(0), mRelation(_consistent ? BVCompareRelation::EQ : BVCompareRelation::NEQ),
		mLhs(), mRhs()
		{}

		/**
		 * Constructs the constraint: _lhs _relation _rhs
		 * @param _lhs The bit-vector term to be used as left-hand side of the constraint.
		 * @param _rhs The bit-vector term to be used as right-hand side of the constraint.
		 * @param _relation The relation symbol to be used for the constraint.
		 */
		BVConstraint(const BVCompareRelation& _relation,
			const BVTerm<Pol>& _lhs, const BVTerm<Pol>& _rhs) :
		mHash((toId(_relation) << 10) ^ (_lhs.hash() << 5) ^ _rhs.hash()), mId(0),
		mRelation(_relation), mLhs(_lhs), mRhs(_rhs)
		{
			assert(_lhs.width() == _rhs.width());
		}

	public:

		static BVConstraint<Pol> create(bool _consistent = true)
		{
			return *(BVConstraintPool<Pol>::getInstance().create(_consistent));
		}

		static BVConstraint<Pol> create(const BVCompareRelation& _relation,
			const BVTerm<Pol>& _lhs, const BVTerm<Pol>& _rhs)
		{
			return *(BVConstraintPool<Pol>::getInstance().create(_relation, _lhs, _rhs));
		}

		/**
		 * @return The bit-vector term being the left-hand side of this constraint.
		 */
		const BVTerm<Pol>& lhs() const
		{
			return mLhs;
		}

		/**
		 * @return The bit-vector term being the right-hand side of this constraint.
		 */
		const BVTerm<Pol>& rhs() const
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

		/**
		 * @return A hash value for this constraint.
		 */
		std::size_t getHash() const
		{
			return mHash;
		}

		/**
		 * Gives the string representation of this bit vector constraint.
		 * @param _withActivity A flag which indicates whether to add the formulas' activity to the result.
		 *                      (not yet supported for bit vector objects, but is passed to subformulae)
		 * @param _resolveUnequal A switch which indicates how to represent the relation symbol for unequal.
		 *                        (Again, only applies to non-bitvector parts of the constraint.
		 *                        For further description see documentation of Formula::toString( .. ))
		 * @param _init The initial string of every row of the result.
		 * @param _oneline A flag indicating whether the constraint shall be printed on one line.
		 * @param _infix A flag indicating whether to print the constraint in infix or prefix notation.
		 * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
		 *                        or with their dedicated names.
		 * @return The resulting string representation of this constraint.
		 */
		std::string toString(bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const
		{
			std::string out = _init + "(";

			if(!_infix)
				out += carl::toString(mRelation) + (_oneline ? " " : "");

			if(!_oneline)
				out += "\n";

			out += mLhs.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
			out += (_oneline ? " " : "\n");

			if(_infix)
				out += _init + carl::toString(mRelation) + (_oneline ? " " : "\n");

			out += mRhs.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
			out += ")";

			return out;
		}

		/**
		 * The output operator of a constraint.
		 * @param _out The stream to print on.
		 * @param _constraint The constraint to be printed.
		 */
		template<typename P>
		friend std::ostream& operator<<(std::ostream& _out, const BVConstraint<P>& _constraint)
		{
			return(_out << _constraint.toString());
		}

		bool operator==(const BVConstraint<Pol>& _other) const
		{
			if(mId != 0 && _other.mId != 0) {
				return mId == _other.mId;
			}

			return
				mRelation == _other.mRelation
				&& mLhs == _other.mLhs
				&& mRhs == _other.mRhs;
		}
		bool operator<(const BVConstraint<Pol>& _other) const
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
	};
} // namespace carl


namespace std
{
	/**
	 * Implements std::hash for bit-vector constraints.
	 */
	template<typename Pol>
	struct hash<carl::BVConstraint<Pol>>
	{
		public:

		/**
		 * @param _constraint The bit-vector constraint to get the hash for.
		 * @return The hash of the given constraint.
		 */
		size_t operator()(const carl::BVConstraint<Pol>& _constraint) const
		{
			return _constraint.hash();
		}
	};
} // namespace std
