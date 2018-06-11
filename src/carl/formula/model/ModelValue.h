#pragma once


#include "../../util/variant_util.h"
#include "../bitvector/BVValue.h"
#include "mvroot/MultivariateRoot.h"
#include "ran/RealAlgebraicNumber.h"
#include "sqrtex/SqrtEx.h"
#include "uninterpreted/SortValue.h"
#include "uninterpreted/UFModel.h"

#include <boost/variant.hpp>

#include <memory>

namespace carl
{
	template<typename Rational, typename Poly>
	class ModelValue;
	template<typename Rational, typename Poly>
	class ModelSubstitution;
	template<typename Rational, typename Poly>
	class ModelMVRootSubstitution;
	template<typename Rational, typename Poly>
	using ModelSubstitutionPtr = std::shared_ptr<ModelSubstitution<Rational,Poly>>;
	template<typename Rational, typename Poly, typename Substitution, typename... Args>
	inline ModelValue<Rational,Poly> createSubstitution(Args&&... args);
	template<typename Rational, typename Poly>
	inline ModelValue<Rational,Poly> createSubstitution(const MultivariateRoot<Poly>& mr);

	/**
	 * This class represents infinity or minus infinity, depending on its flag positive.
	 * The default is minus infinity.
	 */
	struct InfinityValue {
		bool positive = false;
		explicit InfinityValue() = default;
		explicit InfinityValue(bool positive): positive(positive) {}
	};
	
	inline std::string toString(const InfinityValue& iv, bool _infix) {
		if (_infix) {
			std::string result = iv.positive ? "+" : "-";
			result += "infinity";
			return result;
		}
		if (iv.positive) return "infinity";
		return "(- infinity)";
	}
	
	inline std::ostream& operator<<(std::ostream& os, const InfinityValue& iv) {
		return os << (iv.positive ? "+" : "-") << "infinity";
	}
	
	/**
	 * Represent a sum type/variant over the different kinds of values that
	 * can be assigned to the different kinds of variables that exist in
	 * CARL and to use them in a more uniform way,
	 * e.g. a plain "bool", "infinity", a "carl::RealAlgebraicNumber",
	 * a (bitvector) "carl::BVValue" etc.
	 */
	template<typename Rational, typename Poly>
	class ModelValue {
		template<typename R, typename P>
		friend std::ostream& operator<<(std::ostream& os, const ModelValue<R,P>& mv);
		/**
		 * Base type we are deriving from.
		 */
		using Super = boost::variant<bool, Rational, SqrtEx<Poly>, RealAlgebraicNumber<Rational>, BVValue, SortValue, UFModel, InfinityValue, ModelSubstitutionPtr<Rational,Poly>>;
		
		Super mData;
		
		struct VariantExtractor: boost::static_visitor<Super> {
			template<typename T>
			Super operator()(const T& t) const {
				return Super(t);
			}
			Super operator()(const MultivariateRoot<Poly>& mr) const {
				return Super(createSubstitution<Rational,Poly>(mr).asSubstitution());
			}
		};
		template<typename F, typename Return = void>
		struct VariantCaller: boost::static_visitor<Return> {
			F mCallback;
			VariantCaller(F&& f): mCallback(std::forward<F>(f)) {}
			template<typename T>
			auto operator()(const T& t) const {
				return mCallback(t);
			}
		};
		
		
	public:
		/**
		 * Default constructor.
		 */
		ModelValue() = default;

		/**
		 * Initialize the Assignment from some valid type of the underlying variant.
		 */
		template<typename T, typename T2 = typename std::enable_if<convertible_to_variant<T, Super>::value, T>::type>
		ModelValue(const T& _t): mData(_t) {}
		
		template<typename ...Args>
		ModelValue(const boost::variant<Args...>& variant): mData(boost::apply_visitor(VariantExtractor(), variant)) {}
		
		ModelValue(const MultivariateRoot<Poly>& mr): mData(createSubstitution<Rational,Poly>(mr).asSubstitution()) {}
		
		/**
		 * Assign some value to the underlying variant.
		 * @param t Some value.
		 * @return *this.
		 */
		template<typename T>
		ModelValue& operator=(const T& _t) {
			mData = _t;
			return *this;
		}
		template<typename ...Args>
		ModelValue& operator=(const boost::variant<Args...>& variant) {
			mData = boost::apply_visitor(VariantExtractor(), variant);
			return *this;
		}
		ModelValue& operator=(const MultivariateRoot<Poly>& mr) {
			mData = createSubstitution<Rational,Poly>(mr).asSubstitution();
			return *this;
		}
		
		template<typename F, typename Return = void>
		Return visit(F&& f) const {
			return boost::apply_visitor(VariantCaller<F, Return>(std::forward<F>(f)), mData);
		}

		/**
		 * @return true, if the stored value is a bool.
		 */
		bool isBool() const {
			return variant_is_type<bool>(mData);
		}
		
		/**
		 * @return true, if the stored value is a rational.
		 */
		bool isRational() const {
			return variant_is_type<Rational>(mData);
		}
		
		/**
		 * @return true, if the stored value is a square root expression.
		 */
		bool isSqrtEx() const {
			return variant_is_type<SqrtEx<Poly>>(mData);
		}
		
		/**
		 * @return true, if the stored value is a real algebraic number.
		 */
		bool isRAN() const {
			return variant_is_type<RealAlgebraicNumber<Rational>>(mData);
		}
		
		/**
		 * @return true, if the stored value is a bitvector literal.
		 */
		bool isBVValue() const {
			return variant_is_type<BVValue>(mData);
		}

		/**
		 * @return true, if the stored value is a sort value.
		 */
		bool isSortValue() const {
			return variant_is_type<SortValue>(mData);
		}
		
		/**
		 * @return true, if the stored value is a uninterpreted function model.
		 */
		bool isUFModel() const {
			return variant_is_type<UFModel>(mData);
		}
		
		/**
		 * @return true, if the stored value is +infinity.
		 */
		bool isPlusInfinity() const {
			return variant_is_type<InfinityValue>(mData) && boost::get<InfinityValue>(mData).positive;
		}
		/**
		 * @return true, if the stored value is -infinity.
		 */
		bool isMinusInfinity() const {
			return variant_is_type<InfinityValue>(mData) && !boost::get<InfinityValue>(mData).positive;
		}
		
		bool isSubstitution() const {
			return variant_is_type<ModelSubstitutionPtr<Rational,Poly>>(mData);
		}

		/**
		 * @return The stored value as a bool.
		 */
		bool asBool() const {
			assert(isBool());
			return boost::get<bool>(mData);
		}
		
		/**
		 * @return The stored value as a rational.
		 */
		const Rational& asRational() const {
			assert(isRational());
			return boost::get<Rational>(mData);
		}
		
		/**
		 * @return The stored value as a square root expression.
		 */
		const SqrtEx<Poly>& asSqrtEx() const {
			assert(isSqrtEx());
			return boost::get<SqrtEx<Poly>>(mData);
		}
		
		/**
		 * @return The stored value as a real algebraic number.
		 */
		const RealAlgebraicNumber<Rational>& asRAN() const {
			assert(isRAN());
			return boost::get<RealAlgebraicNumber<Rational>>(mData);
		}
		
		/**
		 * @return The stored value as a real algebraic number.
		 */
		const carl::BVValue& asBVValue() const {
			assert(isBVValue());
			return boost::get<carl::BVValue>(mData);
		}

		/**
		 * @return The stored value as a sort value.
		 */
		const SortValue& asSortValue() const {
			assert(isSortValue());
			return boost::get<SortValue>(mData);
		}
		
		/**
		 * @return The stored value as a uninterpreted function model.
		 */
		const UFModel& asUFModel() const {
			assert(isUFModel());
			return boost::get<UFModel>(mData);
		}
		UFModel& asUFModel() {
			assert(isUFModel());
			return boost::get<UFModel>(mData);
		}
		/**
		 * @return The stored value as a infinity value.
		 */
		const InfinityValue& asInfinity() const {
			assert(isPlusInfinity() || isMinusInfinity());
			return boost::get<InfinityValue>(mData);
		}
		
		const ModelSubstitutionPtr<Rational,Poly>& asSubstitution() const {
			assert(isSubstitution());
			return boost::get<ModelSubstitutionPtr<Rational,Poly>>(mData);
		}
		ModelSubstitutionPtr<Rational,Poly>& asSubstitution() {
			assert(isSubstitution());
			return boost::get<ModelSubstitutionPtr<Rational,Poly>>(mData);
		}	
	};

	/**
	 * Check if two Assignments are equal.
	 * Two Assignments are considered equal, if both are either bool or not bool and their value is the same.
	 * 
	 * If both Assignments are not bools, the check may return false although they represent the same value.
	 * If both are numbers in different representations, this comparison is only done as a "best effort".
	 * 
	 * @param lhs First Assignment.
	 * @param rhs Second Assignment.
	 * @return lhs == rhs.
	 */
	template<typename Rational, typename Poly>
	bool operator==(const ModelValue<Rational,Poly>& lhs, const ModelValue<Rational,Poly>& rhs) {
		if (lhs.isBool() && rhs.isBool()) {
			return lhs.asBool() == rhs.asBool();
		} else if (lhs.isRational() && rhs.isRational()) {
			return lhs.asRational() == rhs.asRational();
		} else if (lhs.isSqrtEx() && rhs.isSqrtEx()) {
			return lhs.asSqrtEx() == rhs.asSqrtEx();
		} else if (lhs.isRAN() && rhs.isRAN()) {
			return lhs.asRAN() == rhs.asRAN();
		} else if (lhs.isBVValue() && rhs.isBVValue()) {
			return lhs.asBVValue() == rhs.asBVValue();
		} else if (lhs.isSortValue() && rhs.isSortValue()) {
			return lhs.asSortValue() == rhs.asSortValue();
		} else if (lhs.isUFModel() && rhs.isUFModel()) {
			return lhs.asUFModel() == rhs.asUFModel();
		} else {
			return false;
		}
	}
	
	template<typename R, typename P>
	inline std::ostream& operator<<(std::ostream& os, const ModelValue<R,P>& mv) {
		return os << mv.mData;
	}
}
