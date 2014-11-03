/**
 * @file VariableNamePool.cpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "VariableNamePool.h"

using namespace std;

namespace carl
{
    VariableNamePool::VariableNamePool():
        Singleton(),
        mExternalPrefixInitialized( false ),
        mAuxiliaryBoolVarCounter( 0 ),
        mAuxiliaryRealVarCounter( 0 ),
        mAuxiliaryIntVarCounter( 0 ),
        mExternalVarNamePrefix( "_" ),
        mExternalNamesToVariables(),
        mBooleanVariables()
    {}

    VariableNamePool::~VariableNamePool()
    {}

    void VariableNamePool::clear()
    {
        ARITHMETIC_VAR_LOCK_GUARD
        BOOLEAN_VAR_LOCK_GUARD
        mAuxiliaryRealVarCounter = 0;
        mAuxiliaryIntVarCounter = 0;
        mBooleanVariables.clear();
        mAuxiliaryBoolVarCounter = 0;
        mExternalNamesToVariables.clear();
    }

    Variable VariableNamePool::newVariable( const string& _name, VariableType _domain, bool _parsed )
    {
        if (_domain == VariableType::VT_BOOL) {
                return newBooleanVariable(_name, _parsed);
        } else if (_domain == VariableType::VT_UNINTERPRETED) {
			return newUninterpretedVariable(_name, _parsed);
		} else {
                return newArithmeticVariable(_name, _domain, _parsed);
        }
    }
	
    Variable VariableNamePool::newUninterpretedVariable( const string& _name, bool _parsed )
    {
        UNINTERPRETED_VAR_LOCK_GUARD
        if( _parsed )
        {
            assert( !mExternalPrefixInitialized );
            mParsedVarNames.push_back( _name );
        }
        Variable result = VariablePool::getInstance().getFreshVariable( VariableType::VT_UNINTERPRETED );
        VariablePool::getInstance().setName( result, _name );
        return result;
    }

    Variable VariableNamePool::newArithmeticVariable( const string& _name, VariableType _domain, bool _parsed )
    {
        assert( !_name.empty() );
        assert( _domain == VariableType::VT_REAL || _domain == VariableType::VT_INT );
        // Initialize the prefix for the external representation of internally generated (not parsed) variable names
        if( _parsed )
        {
            assert( !mExternalPrefixInitialized );
            mParsedVarNames.push_back( _name );
        }
        ARITHMETIC_VAR_LOCK_GUARD
        // Create the arithmetic variable
        auto iterBoolPair = mExternalNamesToVariables.insert( pair<string,Variable>( _name, VariablePool::getInstance().getFreshVariable( _domain ) ) );
        assert( iterBoolPair.second );
        VariablePool::getInstance().setName( iterBoolPair.first->second, _name );
        return iterBoolPair.first->second;
    }
    
    Variable VariableNamePool::newBooleanVariable( const string& _name, bool _parsed )
    {
        BOOLEAN_VAR_LOCK_GUARD
        assert( !booleanExistsAlready( _name ) );
        if( _parsed )
        {
            assert( !mExternalPrefixInitialized );
            mParsedVarNames.push_back( _name );
        }
        Variable result = VariablePool::getInstance().getFreshVariable( VariableType::VT_BOOL );
        VariablePool::getInstance().setName( result, _name );
        mBooleanVariables.insert( result );
        return result;
    }

    Variable VariableNamePool::newAuxiliaryBooleanVariable( const std::string& _externalPrefix )
    {
        stringstream out;
        BOOLEAN_VAR_LOCK
        if( !mExternalPrefixInitialized ) initExternalPrefix();
        out << mExternalVarNamePrefix << _externalPrefix << mAuxiliaryBoolVarCounter++;
        BOOLEAN_VAR_UNLOCK
        return newBooleanVariable( out.str() );;
    }
	
    Variable VariableNamePool::newAuxiliaryUninterpretedVariable( const std::string& _externalPrefix )
    {
        stringstream out;
        UNINTERPRETED_VAR_LOCK
        if( !mExternalPrefixInitialized ) initExternalPrefix();
        out << mExternalVarNamePrefix << _externalPrefix << mAuxiliaryUninterpretedVarCounter++;
        UNINTERPRETED_VAR_UNLOCK
        return newUninterpretedVariable( out.str() );;
    }
    
    void VariableNamePool::initExternalPrefix()
    {
        bool foundExternalPrefix = false;
        while( !foundExternalPrefix )
        {
            auto varName = mParsedVarNames.begin(); 
            while( varName != mParsedVarNames.end() )
            {
                unsigned pos = 0;
                while( pos < varName->size() && pos < mExternalVarNamePrefix.size() && varName->at( pos ) == mExternalVarNamePrefix.at( pos ) ) ++pos;
                if( pos == mExternalVarNamePrefix.size() )
                {
                    mExternalVarNamePrefix += "_";
                    break;
                }
                ++varName;
            }
            if( varName == mParsedVarNames.end() ) foundExternalPrefix = true;
        }
        mExternalPrefixInitialized = true;
    }

    const VariableNamePool& variablePool()
    {
        return VariableNamePool::getInstance();
    }

    Variable newVariable( const std::string& _name, VariableType _domain, bool _parsed )
    {
            return VariableNamePool::getInstance().newVariable( _name, _domain, _parsed );
    }

    Variable newRealVariable( const std::string& _name )
    {
        return VariableNamePool::getInstance().newArithmeticVariable( _name, VariableType::VT_REAL );
    }

    Variable newArithmeticVariable( const std::string& _name, VariableType _domain, bool _parsed )
    {
        return VariableNamePool::getInstance().newArithmeticVariable( _name, _domain, _parsed );
    }

    Variable newBooleanVariable( const std::string& _name, bool _parsed )
    {
        return VariableNamePool::getInstance().newBooleanVariable( _name, _parsed );
    }

    Variable newAuxiliaryIntVariable()
    {
        return VariableNamePool::getInstance().newAuxiliaryIntVariable();
    }

    Variable newAuxiliaryIntVariable( const std::string& _varName )
    {
        return VariableNamePool::getInstance().newAuxiliaryIntVariable( _varName );
    }

    Variable newAuxiliaryRealVariable()
    {
        return VariableNamePool::getInstance().newAuxiliaryRealVariable();
    }

    Variable newAuxiliaryRealVariable( const std::string& _varName )
    {
        return VariableNamePool::getInstance().newAuxiliaryRealVariable( _varName );
    }

    Variable newAuxiliaryBooleanVariable()
    {
        return VariableNamePool::getInstance().newAuxiliaryBooleanVariable();
    }
	
    Variable newAuxiliaryUninterpretedVariable()
    {
        return VariableNamePool::getInstance().newAuxiliaryUninterpretedVariable();
    }

    Variable newAuxiliaryVariable(VariableType type)
    {
            switch (type) {
                    case VariableType::VT_REAL: return newAuxiliaryRealVariable();
                    case VariableType::VT_INT: return newAuxiliaryIntVariable();
                    case VariableType::VT_BOOL: return newAuxiliaryBooleanVariable();
                    default:
                            assert(false);
            }
            return newAuxiliaryBooleanVariable();
    }

}    // namespace carl

