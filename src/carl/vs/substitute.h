#pragma once

#include "substitution.h"
#include <bitset>
#include <vector>
#include <carl/formula/Constraint.h>

#include "zeros.h"


/**
 * The maximal number of splits performed, if the left-hand side of a constraints is a product of 
 * polynomials and we split the constraint into constraints over these polynomials according to the
 * relation.
 */
const unsigned MAX_PRODUCT_SPLIT_NUMBER = 64;
/**
 * The maximum number of the resulting combinations when combining according to the method combine.
 * If this number is exceeded, the method return false, and no combining is performed.
 */
const unsigned MAX_NUM_OF_COMBINATION_RESULT = 1025;

namespace carl::vs {
    /// a vector of constraints
    template<typename Poly>
    using constraint_conjunction = std::vector<Constraint<Poly>>;
    /// a vector of vectors of constraints
    template<typename Poly>
    using case_distinction = std::vector<constraint_conjunction<Poly>>;
}

namespace carl::vs::detail {
    using DoubleInterval = carl::Interval<double>;
    using EvalDoubleIntervalMap = std::map<carl::Variable, DoubleInterval>;
    
    /**
     * Combines vectors.
     * @param _toCombine 	The vectors to combine.
     * @param _combination 	The resulting combination.
     * @return false, if the upper limit in the number of combinations resulting by this method is exceeded.
     *                 Note, that this hinders a combinatorial blow up.
     *          true, otherwise.
     */
    template <class combineType> 
    bool combine( const std::vector< std::vector< std::vector< combineType > > >& _toCombine, std::vector< std::vector< combineType > >& _combination );
    
    /**
     * Simplifies a disjunction of conjunctions of constraints by deleting consistent
     * constraint and inconsistent conjunctions of constraints. If a conjunction of
     * only consistent constraints exists, the simplified disjunction contains one
     * empty conjunction.
     * @param _toSimplify The disjunction of conjunctions to simplify.
     */
    template<typename Poly>
    void simplify( case_distinction<Poly>& );
        
    /**
     * Simplifies a disjunction of conjunctions of constraints by deleting consistent
     * constraint and inconsistent conjunctions of constraints. If a conjunction of
     * only consistent constraints exists, the simplified disjunction contains one
     * empty conjunction.
     * @param _toSimplify   The disjunction of conjunctions to simplify.
     * @param _conflictingVars
     * @param _solutionSpace
     */
    template<typename Poly>
    void simplify( case_distinction<Poly>&, carl::Variables&, const detail::EvalDoubleIntervalMap& );
    
    /**
     * Splits all constraints in the given disjunction of conjunctions of constraints having a non-trivial 
     * factorization into a set of constraints which compare the factors instead.
     * @param _toSimplify The disjunction of conjunctions of the constraints to split.
     * @param _onlyNeq A flag indicating that only constraints with the relation symbol != are split.
     * @return false, if the upper limit in the number of combinations resulting by this method is exceeded.
     *                 Note, that this hinders a combinatorial blow up.
     *          true, otherwise.
     */
    template<typename Poly>
    bool splitProducts( case_distinction<Poly>&, bool = false );
    
    /**
     * Splits all constraints in the given conjunction of constraints having a non-trivial 
     * factorization into a set of constraints which compare the factors instead.
     * @param _toSimplify The conjunction of the constraints to split.
     * @param _result The result, being a disjunction of conjunctions of constraints.
     * @param _onlyNeq A flag indicating that only constraints with the relation symbol != are split.
     * @return false, if the upper limit in the number of combinations resulting by this method is exceeded.
     *                 Note, that this hinders a combinatorial blow up.
     *          true, otherwise.
     */
    template<typename Poly>
    bool splitProducts( const constraint_conjunction<Poly>&, case_distinction<Poly>&, bool = false );
    
    /**
     * Splits the given constraint into a set of constraints which compare the factors of the
     * factorization of the constraints considered polynomial.
     * @param _constraint A pointer to the constraint to split.
     * @param _onlyNeq A flag indicating that only constraints with the relation symbol != are split.
     * @return The resulting disjunction of conjunctions of constraints, which is semantically equivalent to
     *          the given constraint.
     */
    template<typename Poly>
    case_distinction<Poly> splitProducts( const Constraint<Poly>&, bool = false );
    
    template<typename Poly>
    void splitSosDecompositions( case_distinction<Poly>& );
    
    /**
     * For a given constraint f_1*...*f_n ~ 0 this method computes all combinations of constraints
     * f_1 ~_1 0 ... f_n ~_n 0 such that 
     * 
     *          f_1 ~_1 0 and ... and f_n ~_n 0   iff   f_1*...*f_n ~ 0 
     * holds.
     * @param _constraint A pointer to the constraint to split this way.
     * @return The resulting combinations.
     */
    template<typename Poly>
    case_distinction<Poly> getSignCombinations( const Constraint<Poly>& );
    
    /**
     * @param _length The maximal length of the bit strings with odd parity to compute.
     * @param _strings All bit strings of length less or equal the given length with odd parity.
     */
    void getOddBitStrings( size_t _length, std::vector< std::bitset<MAX_PRODUCT_SPLIT_NUMBER> >& _strings );
        
    /**
     * @param _length The maximal length of the bit strings with even parity to compute.
     * @param _strings All bit strings of length less or equal the given length with even parity.
     */
    void getEvenBitStrings( size_t _length, std::vector< std::bitset<MAX_PRODUCT_SPLIT_NUMBER> >& _strings );
    
    /**
     * Prints the given disjunction of conjunction of constraints.
     * @param _substitutionResults The disjunction of conjunction of constraints to print.
     */
    template<typename Poly>
    void print( case_distinction<Poly>& _substitutionResults );
    
    /**
     * Applies a substitution to a constraint and stores the results in the given vector.
     * @param _cons   The constraint to substitute in.
     * @param _subs   The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution.
     * @return false, if the upper limit in the number of combinations in the result of the substitution is exceeded.
     *                 Note, that this hinders a combinatorial blow up.
     *          true, otherwise.
     */
    template<typename Poly>
    bool substitute( const Constraint<Poly>&, const substitution<Poly>&, case_distinction<Poly>&, bool _accordingPaper, carl::Variables&, const detail::EvalDoubleIntervalMap& );
    
    /**
     * Applies a substitution of a variable to a term, which is not minus infinity nor a to an square root expression plus an infinitesimal.
     * @param _cons   The constraint to substitute in.
     * @param _subs   The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     * @param _conflictingVariables If a conflict with the given solution space occurs, the variables being part of this conflict are stored in this
     *                               container.
     * @param _solutionSpace The solution space in form of double intervals of the variables occurring in the given constraint.
     */
    template<typename Poly>
    bool substituteNormal( const Constraint<Poly>& _cons, const substitution<Poly>& _subs, case_distinction<Poly>& _result, bool _accordingPaper, carl::Variables& _conflictingVariables, const detail::EvalDoubleIntervalMap& _solutionSpace );
    
    /**
     * Sub-method of substituteNormalSqrt, where applying the substitution led to a term
     * containing a square root. The relation symbol of the constraint to substitute is "=".
     *
     *                              (_q+_r*sqrt(_radicand))
     * The term then looks like:    ------------------
     *                                      _s
     *
     * @param _radicand The radicand of the square root.
     * @param _q The summand not containing the square root.
     * @param _r The coefficient of the radicand.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     */
    template<typename Poly>
    bool substituteNormalSqrtEq( const Poly& _radicand, const Poly& _q, const Poly& _r, case_distinction<Poly>& _result, bool _accordingPaper );
    
    /**
     * Sub-method of substituteNormalSqrt, where applying the substitution led to a term
     * containing a square root. The relation symbol of the constraint to substitute is "!=".
     *
     *                              (_q+_r*sqrt(_radicand))
     * The term then looks like:    -----------------------
     *                                      _s
     *
     * @param _radicand The radicand of the square root.
     * @param _q The summand not containing the square root.
     * @param _r The coefficient of the radicand.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     */
    template<typename Poly>
    bool substituteNormalSqrtNeq( const Poly& _radicand, const Poly& _q, const Poly& _r, case_distinction<Poly>& _result, bool _accordingPaper );
    
    /**
     * Sub-method of substituteNormalSqrt, where applying the substitution led to a term
     * containing a square root. The relation symbol of the constraint to substitute is less.
     *
     *                              (_q+_r*sqrt(_radicand))
     * The term then looks like:    ----------------------
     *                                      _s
     *
     * @param _radicand The radicand of the square root.
     * @param _q The summand not containing the square root.
     * @param _r The coefficient of the radicand.
     * @param _s The denominator of the expression containing the square root.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     */
    template<typename Poly>
    bool substituteNormalSqrtLess( const Poly& _radicand, const Poly& _q, const Poly& _r, const Poly& _s, case_distinction<Poly>& _result, bool _accordingPaper );
    
    /**
     * Sub-method of substituteNormalSqrt, where applying the substitution led to a term
     * containing a square root. The relation symbol of the constraint to substitute is less or equal.
     *
     *                              (_q+_r*sqrt(_radicand))
     * The term then looks like:    ----------------------
     *                                      _s
     *
     * @param _radicand The radicand of the square root.
     * @param _q The summand not containing the square root.
     * @param _r The coefficient of the radicand.
     * @param _s The denominator of the expression containing the square root.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     */
    template<typename Poly>
    bool substituteNormalSqrtLeq( const Poly& _radicand, const Poly& _q, const Poly& _r, const Poly& _s, case_distinction<Poly>& _result, bool _accordingPaper );
    
    /**
     * Applies the given substitution to the given constraint, where the substitution
     * is of the form [x -> t+epsilon] with x as the variable and c and b polynomials in
     * the real theory excluding x. The constraint is of the form "f(x) \rho 0" with
     * \rho element of {=,!=,<,>,<=,>=} and k as the maximum degree of x in f.
     *
     * @param _cons The constraint to substitute in.
     * @param _subs The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     * @param _conflictingVariables If a conflict with the given solution space occurs, the variables being part of this conflict are stored in this
     *                               container.
     * @param _solutionSpace The solution space in form of double intervals of the variables occurring in the given constraint.
     */
    template<typename Poly>
    bool substitutePlusEps( const Constraint<Poly>& _cons, const substitution<Poly>& _subs, case_distinction<Poly>& _result, bool _accordingPaper, carl::Variables& _conflictingVariables, const detail::EvalDoubleIntervalMap& _solutionSpace );
    
    /**
     * Sub-method of substituteEps, where one of the gradients in the
     * point represented by the substitution must be negative if the given relation is less
     * or positive if the given relation is greater. 
     * @param _cons The constraint to substitute in.
     * @param _subs The substitution to apply.
     * @param _relation The relation symbol, deciding whether the substitution result must be negative or positive.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _accordingPaper A flag that indicates whether to apply the virtual substitution rules according to the paper "Quantifier elimination
     *                        for real algebra - the quadratic case and beyond." by Volker Weispfenning (true) or in an adapted way which omits a higher
     *                        degree in the result by splitting the result in more cases (false).
     * @param _conflictingVariables If a conflict with the given solution space occurs, the variables being part of this conflict are stored in this
     *                               container.
     * @param _solutionSpace The solution space in form of double intervals of the variables occurring in the given constraint.
     */
    template<typename Poly>
    bool substituteEpsGradients( const Constraint<Poly>& _cons, const substitution<Poly>& _subs, const carl::Relation _relation, case_distinction<Poly>&, bool _accordingPaper, carl::Variables& _conflictingVariables, const detail::EvalDoubleIntervalMap& _solutionSpace );
    
    /**
     * Applies the given substitution to the given constraint, where the substitution
     * is of the form [x -> -infinity] with x as the variable and c and b polynomials in
     * the real theory excluding x. The constraint is of the form "f(x) \rho 0" with
     * \rho element of {=,!=,<,>,<=,>=} and k as the maximum degree of x in f.
     * @param _cons   The constraint to substitute in.
     * @param _subs   The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     * @param _conflictingVariables If a conflict with the given solution space occurs, the variables being part of this conflict are stored in this
     *                               container.
     * @param _solutionSpace The solution space in form of double intervals of the variables occurring in the given constraint.
     */
    template<typename Poly>
    void substituteInf( const Constraint<Poly>& _cons, const substitution<Poly>& _subs, case_distinction<Poly>& _result, carl::Variables& _conflictingVariables, const detail::EvalDoubleIntervalMap& _solutionSpace );
    
    /**
     * Applies the given substitution to the given constraint, where the substitution
     * is of the form [x -> +/-infinity] with x as the variable and c and b polynomials in
     * the real theory excluding x. The constraint is of the form "a*x^2+bx+c \rho 0",
     * where \rho is less or greater.
     * @param _cons   The constraint to substitute in.
     * @param _subs   The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     */
    template<typename Poly>
    void substituteInfLessGreater( const Constraint<Poly>& _cons, const substitution<Poly>& _subs, case_distinction<Poly>& _result );
    
    /**
     * Deals with the case, that the left hand side of the constraint to substitute is
     * a trivial polynomial in the variable to substitute.
     * The constraints left hand side then should look like:   ax^2+bx+c
     * @param _cons   The constraint to substitute in.
     * @param _subs   The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     */
    template<typename Poly>
    void substituteTrivialCase( const Constraint<Poly>& _cons, const substitution<Poly>& _subs, case_distinction<Poly>& _result );
    
    /**
     * Deals with the case, that the left hand side of the constraint to substitute is
     * not a trivial polynomial in the variable to substitute.
     * The constraints left hand side then should looks like:   ax^2+bx+c
     * @param _cons   The constraint to substitute in.
     * @param _subs   The substitution to apply.
     * @param _result The vector, in which to store the results of this substitution. It is semantically a disjunction of conjunctions of constraints.
     */
    template<typename Poly>
    void substituteNotTrivialCase( const Constraint<Poly>&, const substitution<Poly>&, case_distinction<Poly>& );
    
}

namespace carl::vs {
    /**
     * Applies a substitution to a constraint.
     * @param cons   The constraint to substitute in.
     * @param subs   The substitution to apply.
     * @return std::nullopt, if the upper limit in the number of combinations in the result of the substitution is exceeded.
     *                 Note, that this hinders a combinatorial blow up.
     *          Thr substitution result, otherwise.
     */
    template<typename Poly>
    std::optional<case_distinction<Poly>> substitute(const Constraint<Poly>& cons, const substitution<Poly>& subs) {
        case_distinction<Poly> subres;
        carl::Variables dummy_vars; // we do not make use of this feature here
        detail::EvalDoubleIntervalMap dummy_map; // we do not make use of this feature here
        if (!detail::substitute(cons, subs, subres, false, dummy_vars, dummy_map)) {
            return std::nullopt;
        } else {
            return subres;
        }
    }

    /**
     * Applies a substitution to a variable comparison.
     * @param varcomp   The variable comparison to substitute in.
     * @param subs   The substitution to apply.
     * @return std::nullopt, if the upper limit in the number of combinations in the result of the substitution is exceeded
     *                  or the substitution cannot be applied.
     *                 Note, that this hinders a combinatorial blow up.
     *          Thr substitution result, otherwise.
     */
    template<typename Poly>
    std::optional<std::variant<case_distinction<Poly>, VariableComparison<Poly>>> substitute(const VariableComparison<Poly>& varcomp, const substitution<Poly>& subs) {
        if (std::holds_alternative<MultivariateRoot<Poly>>(varcomp.value()) && std::get<MultivariateRoot<Poly>>(varcomp.value()).poly().has(subs.variable())) {
            // Substitution variable occurs in MVRoot's polynomial
            return std::nullopt;
        }

        if (varcomp.var() != subs.variable()) {
            return varcomp;
        }

        carl::Variable subVar1 = carl::freshRealVariable("__subVar1");
        carl::Variable subVar2 = carl::freshRealVariable("__subVar2");
        carl::Relation varcompRelation = varcomp.negated() ? carl::inverse(varcomp.relation()) : varcomp.relation();

        if (subs.term().is_normal() || subs.term().is_plus_eps()) {
            // convert MVRoot/RAN to SqrtExpression with side conditions
            std::vector<zero<Poly>> zeros;
            if (!gather_zeros(varcomp, subs.variable(), zeros)) {
                return std::nullopt;
            }
            assert(zeros.size() == 1);

            // calculate subVar1-subVar2 ~ 0 [subs.term()//subVar1][zero//subVar2]
            Constraint<Poly> subConstraint(Poly(subVar1) - subVar2, varcompRelation);
            substitution<Poly> subSub1(subVar1, subs.term());
            substitution<Poly> subSub2(subVar2, substitution_term<Poly>::normal(zeros[0].sqrt_ex));

            auto subRes1 = substitute(subConstraint, subSub1);
            assert(subRes1);
            case_distinction<Poly> result;
            for (const auto& vcase : *subRes1) {
                std::vector<case_distinction<Poly>> subresults;
                size_t num_cases = 1;
                for (const auto& constr : vcase) {
                    auto subRes2 = substitute(constr, subSub2);
                    assert(subRes2);
                    subresults.push_back(*subRes2);
                    num_cases = num_cases * subRes2->size();
                }

                // distributive law                
                for (size_t i = 0; i < num_cases; i++) {
                    auto temp = i;
                    result.emplace_back(zeros[0].side_condition.begin(), zeros[0].side_condition.end());
                    for (auto const& vec : subresults) {
                        auto index = temp % vec.size();
                        temp /= vec.size();
                        result.back().insert(result.back().end(), vec[index].begin(), vec[index].end());
                    }
                }
            }
            return result;
        } else if(subs.term().is_minus_infty() ) {
            return substitute(Constraint<Poly>(subVar1, varcompRelation), substitution<Poly>(subVar1, substitution_term<Poly>::minus_infty()));
        } else {
            return std::nullopt;
        }
    }
}

#include "substitute.tpp"