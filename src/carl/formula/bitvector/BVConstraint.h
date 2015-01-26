/**
 * @file BVConstraint.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVCompareRelation.h"
#include "BVTerm.h"

namespace carl
{

	template<typename Pol>
	class BVConstraint
	{
		friend class Pool<BVConstraint<Pol>>;

	private:
		/// The hash value.
		size_t mHash;
		/// The unique id.
		size_t mId;

		/// The relation for comparing left- and right-hand side.
		const BVCompareRelation mRelation;
		/// The left-hand side of the (in)equality.
		const BVTerm<Pol>& mLhs;
		/// The right-hand size of the (in)equality.
		const BVTerm<Pol>& mRhs;

	public:

		/**
		 * Constructs the constraint: _lhs _relation _rhs
		 * @param _lhs The bit-vector term to be used as left-hand side of the constraint.
		 * @param _rhs The bit-vector term to be used as right-hand side of the constraint.
		 * @param _relation The relation symbol to be used for the constraint.
		 */
		BVConstraint(const BVCompareRelation& _relation,
			const BVTerm<Pol>& _lhs, const BVTerm<Pol>& _rhs) :
		mRelation(_relation), mLhs(_lhs), mRhs(_rhs),
		mHash((toId(_relation) << 10) ^ (_lhs.hash() << 5) ^ _rhs.hash())
		{
			assert(_lhs.width() == _rhs.width());
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
		unsigned id() const
		{
			return mId;
		}

		/**
		 * @return A hash value for this constraint.
		 */
		size_t getHash() const
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
			return mId == _other.mId; // TODO: Make sure this also works if any mId is not set
		}

		size_t hash() const
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
