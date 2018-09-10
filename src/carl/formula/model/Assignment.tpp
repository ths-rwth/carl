/**
 * Class to create a state object.
 * @author Florian Corzilius
 * @since 2014-01-14
 * @version 2014-01-14
 */

#include "Assignment.h"
#include "uninterpreted/SortValueManager.h"

namespace carl
{
    template<typename Rational, typename Poly>
    bool getRationalAssignmentsFromModel( const Model<Rational,Poly>& _model, std::map<Variable,Rational>& _rationalAssigns )
    {
        bool result = true;
        for( auto ass = _model.begin(); ass != _model.end(); ++ass )
        {
            if (ass->second.isRational())
            {
                _rationalAssigns.insert( _rationalAssigns.end(), std::make_pair(ass->first.asVariable(), ass->second.asRational()));
            }
            else if (ass->second.isSqrtEx())
            {
                if( ass->second.asSqrtEx().isConstant() && !ass->second.asSqrtEx().hasSqrt() )
                {
                    assert( ass->first.isVariable() );
                    Rational value = ass->second.asSqrtEx().constantPart().constantPart()/ass->second.asSqrtEx().denominator().constantPart();
                    assert( !(ass->first.asVariable().getType() == carl::VariableType::VT_INT) || carl::isInteger( value ) );
                    _rationalAssigns.insert( _rationalAssigns.end(), std::make_pair(ass->first.asVariable(), value));
                }
                else
                {
                    result = false;
                }
            }
            else if (ass->second.isRAN())
            {
                if (ass->second.asRAN().isNumeric())
                {
                    assert( ass->first.isVariable() );
                    _rationalAssigns.insert( _rationalAssigns.end(), std::make_pair(ass->first.asVariable(), ass->second.asRAN().value()) );
                }
            }
        }
        return result;
    }

	template<typename Rational, typename Poly>
    unsigned satisfies( const Model<Rational,Poly>& _assignment, const Formula<Poly>& _formula )
    {
        std::map<Variable,Rational> rationalAssigns;
        getRationalAssignmentsFromModel( _assignment, rationalAssigns );
        std::map<carl::BVVariable, carl::BVTerm> bvAssigns;
        for( auto& varAndValue : _assignment )
        {
            if( varAndValue.first.isBVVariable() )
            {
                assert(varAndValue.second.isBVValue());
                carl::BVTerm replacement(carl::BVTermType::CONSTANT, varAndValue.second.asBVValue());
                bvAssigns[varAndValue.first.asBVVariable()] = replacement;
            }
        }
        return satisfies( _assignment, rationalAssigns, bvAssigns, _formula );
    }

	template<typename Rational, typename Poly>
    bool isPartOf( const std::map<Variable,Rational>& _assignment, const Model<Rational,Poly>& _model )
    {
        auto assIter = _assignment.begin();
        auto modIter = _model.begin();
        while( assIter != _assignment.end() && modIter != _model.end() )
        {
            if( modIter->first < assIter->first )
            {
                ++assIter;
            }
            else if( assIter->first < modIter->first )
            {
                return false;
            }
            else
            {
                ++assIter;
                ++modIter;
            }
        }
        return assIter == _assignment.end();
    }

  template<typename Rational, typename Poly>
    SortValue satisfiesUF( const Model<Rational,Poly>& _model, const UFInstance _ufi )
    {
      auto iter = _model.find( _ufi.uninterpretedFunction() );
      if( iter == _model.end() )
          return 2;
      assert( iter->second.isUFModel() );
      const UFModel& ufm = iter->second.asUFModel();
      std::vector<SortValue> inst;
      for( const carl::UTerm& arg : _ufi.args() )
      {
          if(arg.isUVariable()) {
            auto iterB = _model.find( arg() );
            if( iterB == _model.end() )
                return 2;
            assert( iterB->second.isSortValue() );
            inst.push_back( iterB->second.asSortValue() );
          } else if(arg.isUFInstance()) {
            inst.push_back(satisfiesUF(_model, arg.asUFInstance()));
          }
      }
      SortValue sv = ufm.get( inst );
      return sv;
    }

	template<typename Rational, typename Poly>
    unsigned satisfies( const Model<Rational,Poly>& _model, const std::map<Variable,Rational>& _assignment, const std::map<carl::BVVariable, carl::BVTerm>& _bvAssigns, const Formula<Poly>& _formula )
    {
        assert( isPartOf( _assignment, _model ) );
        switch( _formula.getType() )
        {
            case carl::FormulaType::TRUE:
            {
                return 1;
            }
            case carl::FormulaType::FALSE:
            {
                return 0;
            }
            case carl::FormulaType::BOOL:
            {
                auto ass = _model.find( _formula.boolean() );
                return ass == _model.end() ? 2 : (ass->second.asBool() ? 1 : 0);
            }
            case carl::FormulaType::CONSTRAINT:
            {
                return _formula.constraint().satisfiedBy( _assignment );
            }
			case carl::FormulaType::BITVECTOR:
            {
                carl::FormulaSubstitutor<Formula<Poly>> substitutor;
                Formula<Poly> substituted = substitutor.substitute(_formula, _bvAssigns);
                if(substituted.isTrue())
                    return 1;
                else if(substituted.isFalse())
                    return 0;
                return 2;
            }
            case carl::FormulaType::NOT:
            {
                switch( satisfies( _model, _assignment, _bvAssigns, _formula.subformula() ) )
                {
                    case 0:
                        return 1;
                    case 1:
                        return 0;
                    default:
                        return 2;
                }
            }
            case carl::FormulaType::OR:
            {
                unsigned result = 0;
                for( const Formula<Poly>& subFormula : _formula.subformulas() )
                {
                    switch( satisfies( _model, _assignment, _bvAssigns, subFormula ) )
                    {
                        case 0:
                            break;
                        case 1:
                            return 1;
                        default:
                            if( result != 2 ) result = 2;
                    }
                }
                return result;
            }
            case carl::FormulaType::AND:
            {
                unsigned result = 1;
                for( const Formula<Poly>& subFormula : _formula.subformulas() )
                {
                    switch( satisfies( _model, _assignment, _bvAssigns, subFormula ) )
                    {
                        case 0:
                            return 0;
                        case 1:
                            break;
                        default:
                            if( result != 2 ) result = 2;
                    }
                }
                return result;
            }
            case carl::FormulaType::IMPLIES:
            {
                unsigned result = satisfies( _model, _assignment, _bvAssigns, _formula.premise() );
                if( result == 0 ) return 1;
                switch( satisfies( _model, _assignment, _bvAssigns, _formula.conclusion() ) )
                {
                    case 0:
                        return result == 1 ? 0 : 2;
                    case 1:
                        return 1;
                    default:
                        return 2;
                }
            }
            case carl::FormulaType::ITE:
            {
                unsigned result = satisfies( _model, _assignment, _bvAssigns, _formula.condition() );
                switch( result )
                {
                    case 0:
                        return satisfies( _model, _assignment, _bvAssigns, _formula.secondCase() );
                    case 1:
                        return satisfies( _model, _assignment, _bvAssigns, _formula.firstCase() );
                    default:
                        return 2;
                }
            }
            case carl::FormulaType::IFF:
            {
                auto subFormula = _formula.subformulas().begin();
                unsigned result = satisfies( _model, _assignment, _bvAssigns, *subFormula );
                bool containsTrue = (result == 1 ? true : false);
                bool containsFalse = (result == 0 ? true : false);
                ++subFormula;
                while( subFormula != _formula.subformulas().end() )
                {
                    unsigned resultTmp = satisfies( _model, _assignment, _bvAssigns, *subFormula );
                    switch( resultTmp )
                    {
                        case 0:
                            containsFalse = true;
                            break;
                        case 1:
                            containsTrue = true;
                            break;
                        default:
                            result = 2;
                    }
                    if( containsFalse && containsTrue )
                        return 0;
                    ++subFormula;
                }
                return (result == 2 ? 2 : 1);
            }
            case carl::FormulaType::XOR:
            {
                auto subFormula = _formula.subformulas().begin();
                unsigned result = satisfies( _model, _assignment, _bvAssigns, *subFormula );
                if( result == 2 ) return 2;
                ++subFormula;
                while( subFormula != _formula.subformulas().end() )
                {
                    unsigned resultTmp = satisfies( _model, _assignment, _bvAssigns, *subFormula );
                    if( resultTmp == 2 ) return 2;
                    result = resultTmp != result;
                    ++subFormula;
                }
                return result;
            }
            case carl::FormulaType::EXISTS:
            {
                ///@todo do something here
                return 2;
            }
            case carl::FormulaType::FORALL:
            {
                ///@todo do something here
                return 2;
            }
            case carl::FormulaType::UEQ:
            {
                const carl::UEquality& eq = _formula.uequality();
                std::size_t lhsResult = 0;
                std::size_t rhsResult = 0;
                // get sortvalue for lhs and rhs
                if( eq.lhs().isUVariable() )
                {
                    auto iter = _model.find( eq.lhs().asUVariable() );
                    if( iter == _model.end() )
                        return 2;
                    assert( iter->second.isSortValue() );
                    lhsResult = iter->second.asSortValue().id();
                }
                else
                {
                    SortValue sv = satisfiesUF(_model, eq.lhs().asUFInstance());
                    if( sv == defaultSortValue( sv.sort() ) )
                        return 2;
                    lhsResult = sv.id();
                }
                if( eq.rhs().isUVariable() )
                {
                    auto iter = _model.find( eq.rhs(().asUVariable() );
                    if( iter == _model.end() )
                        return 2;
                    assert( iter->second.isSortValue() );
                    rhsResult = iter->second.asSortValue().id();
                }
                else
                {
                    SortValue sv = satisfiesUF(_model, eq.rhs().asUFInstance());
                    if( sv == defaultSortValue( sv.sort() ) )
                        return 2;
                    rhsResult = sv.id();
                }
                // check eq.negated() <=> sv(lhs) != sv(rhs)
                return eq.negated() ? lhsResult != rhsResult : lhsResult == rhsResult;
            }
            default:
            {
                assert( false );
                std::cerr << "Undefined operator!" << std::endl;
                return 2;
            }
        }
    }

	template<typename Rational, typename Poly>
    void getDefaultModel( Model<Rational,Poly>& /*_defaultModel*/, const carl::UEquality& /*_constraint*/, bool /*_overwrite*/, size_t /*_seed*/ )
    {
        assert(false);
    }

	template<typename Rational, typename Poly>
    void getDefaultModel( Model<Rational,Poly>& _defaultModel, const carl::BVTerm& _bvTerm, bool _overwrite, size_t _seed )
    {
        if( _bvTerm.type() == carl::BVTermType::VARIABLE )
        {
            auto ass = _defaultModel.find( _bvTerm.variable() );
            if( ass == _defaultModel.end() )
            {
				_defaultModel.emplace(_bvTerm.variable(), carl::BVValue(_bvTerm.variable().width()));
            }
            else
            {
                // TODO: something with the seed
            }
        }
        else if( carl::typeIsUnary( _bvTerm.type() ) )
            getDefaultModel( _defaultModel, _bvTerm.operand(), _overwrite, _seed );
        else if( carl::typeIsBinary( _bvTerm.type() ) )
        {
            getDefaultModel( _defaultModel, _bvTerm.first(), _overwrite, _seed );
            getDefaultModel( _defaultModel, _bvTerm.second(), _overwrite, _seed );
        }
        else if( _bvTerm.type() == carl::BVTermType::EXTRACT )
            getDefaultModel( _defaultModel, _bvTerm.operand(), _overwrite, _seed );
    }

	template<typename Rational, typename Poly>
    void getDefaultModel( Model<Rational,Poly>& _defaultModel, const Constraint<Poly>& _constraint, bool /*_overwrite*/, size_t /*_seed*/ )
    {
        for( carl::Variable::Arg var : _constraint.variables() )
        {
            auto ass = _defaultModel.find( var );
            if( ass == _defaultModel.end() )
            {
                _defaultModel.emplace(var, 0);
            }
            else
            {
                // TODO: something with the seed
            }
        }
    }

	template<typename Rational, typename Poly>
    void getDefaultModel( Model<Rational,Poly>& _defaultModel, const Formula<Poly>& _formula, bool _overwrite, size_t _seed )
    {
        switch( _formula.getType() )
        {
            case carl::FormulaType::TRUE:
            case carl::FormulaType::FALSE:
            case carl::FormulaType::BOOL:
            {
                auto ass = _defaultModel.find( _formula.boolean() );
                if( ass == _defaultModel.end() )
                {
                    _defaultModel.emplace(_formula.boolean(), false);
                }
                else
                {
                    // TODO: something with the seed
                }
                return;
            }
            case carl::FormulaType::CONSTRAINT:
                getDefaultModel( _defaultModel, _formula.constraint(), _overwrite, _seed );
				return;
            case carl::FormulaType::BITVECTOR:
                getDefaultModel( _defaultModel, _formula.bvConstraint().lhs(), _overwrite, _seed );
                getDefaultModel( _defaultModel, _formula.bvConstraint().rhs(), _overwrite, _seed );
                return;
            case carl::FormulaType::UEQ:
                getDefaultModel( _defaultModel, _formula.uequality(), _overwrite, _seed );
				return;
            case carl::FormulaType::NOT:
                getDefaultModel( _defaultModel, _formula.subformula(), _overwrite, _seed );
				return;
            default:
                assert( _formula.isNary() );
                for (const auto& subFormula: _formula.subformulas() )
                    getDefaultModel(_defaultModel, subFormula, _overwrite, _seed);
        }
    }

	template<typename Rational, typename Poly>
	Formula<Poly> representingFormula(const ModelVariable& mv, const Model<Rational,Poly>& model) {
		auto it = model.find(mv);
		assert(it != model.end());
		return representingFormula(mv, it->second);
	}
	template<typename Rational, typename Poly>
	Formula<Poly> representingFormula(const ModelVariable& mv, const ModelValue<Rational,Poly>& val) {
		if (val.isBool()) {
			assert(mv.isVariable());
			if (val.isBool()) return Formula<Poly>(mv.asVariable());
			else return Formula<Poly>(FormulaType::NOT, Formula<Poly>(mv.asVariable()));
		} else if (val.isRational()) {
			assert(mv.isVariable());
			return Formula<Poly>(VariableAssignment<Poly>(mv.asVariable(), val.asRational()));
		} else if (val.isSqrtEx()) {
			assert(false);
		} else if (val.isRAN()) {
			return Formula<Poly>(VariableAssignment<Poly>(mv.asVariable(), val.asRAN()));
		} else if (val.isBVValue()) {
			assert(false);
		} else if (val.isSortValue()) {
			assert(false);
		} else if (val.isUFModel()) {
			assert(false);
		} else if (val.isSubstitution()) {
			return val.asSubstitution()->representingFormula(mv);
		}
		assert(false);
		return Formula<Poly>(FormulaType::FALSE);
	}
}
