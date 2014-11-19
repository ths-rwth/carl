/**
 * @file VariableNamePool.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include <mutex>
#include <sstream>

#include "../util/Singleton.h"
#include "../util/Common.h"
#include "../core/VariablePool.h"

namespace carl
{
    class VariableNamePool : public Singleton<VariableNamePool>
    {
        friend Singleton<VariableNamePool>;
        private:
            // Members:

            /// A flag indicating whether the prefix of the internally created external variable names has already been initialized.
            bool mExternalPrefixInitialized;
            /// A counter for the auxiliary Boolean valued variables.
            unsigned mAuxiliaryBoolVarCounter;
            /// A counter for the auxiliary real valued variables.
            unsigned mAuxiliaryRealVarCounter;
            /// A counter for the auxiliary integer valued variables.
            unsigned mAuxiliaryIntVarCounter;
			/// A counter for the auxiliary uninterpreted variables.
            unsigned mAuxiliaryUninterpretedVarCounter;
            /// Mutex to avoid multiple access to the map of arithmetic variables
            mutable std::mutex mMutexArithmeticVariables;
            /// Mutex to avoid multiple access to the set of Boolean variables
            mutable std::mutex mMutexBooleanVariables;
			/// Mutex to avoid multiple access to the set of Uninterpreted variables
            mutable std::mutex mMutexUninterpretedVariables;
            /// The external prefix for a variable.
            std::string mExternalVarNamePrefix;
            /// The map of external variable names to internal variable names.
            std::map<std::string,Variable> mExternalNamesToVariables;
            /// The collection of Boolean variables in use.
            Variables mBooleanVariables;
            /// All external variable names which have been created during parsing.
            std::vector<std::string> mParsedVarNames;
            
            #define ARITHMETIC_VAR_LOCK_GUARD std::lock_guard<std::mutex> lock2( mMutexArithmeticVariables );
            #define ARITHMETIC_VAR_LOCK mMutexArithmeticVariables.lock();
            #define ARITHMETIC_VAR_UNLOCK mMutexArithmeticVariables.unlock();
            #define BOOLEAN_VAR_LOCK_GUARD std::lock_guard<std::mutex> lock3( mMutexBooleanVariables );
            #define BOOLEAN_VAR_LOCK mMutexBooleanVariables.lock();
            #define BOOLEAN_VAR_UNLOCK mMutexBooleanVariables.unlock();
			#define UNINTERPRETED_VAR_LOCK_GUARD std::lock_guard<std::mutex> lock4( mMutexUninterpretedVariables );
            #define UNINTERPRETED_VAR_LOCK mMutexUninterpretedVariables.lock();
            #define UNINTERPRETED_VAR_UNLOCK mMutexUninterpretedVariables.unlock();

        protected:
            
            /**
             * Constructor of the variable pool.
             */
            VariableNamePool();

        public:

            /**
             * Destructor.
             */
            ~VariableNamePool();

            /**
             * Returns all constructed Boolean variables. Note, that it does not
             * return the reference to the member, but a copy of it instead. This is
             * due to mutual exclusion and an expensive operation which should only
             * used for debugging or outputting purposes.
             * @return All constructed Boolean variables.
             */
            Variables booleanVariables() const
            {
                return mBooleanVariables;
            }
            
            /**
             * Returns all constructed arithmetic variables. This method constructs a new
             * container of the demanded variables due to mutual exclusion which forms an
             * expensive operation and should only used for debugging or outputting purposes.
             * @return All constructed arithmetic variables.
             */
            Variables arithmeticVariables() const
            {
                Variables result = Variables();
                for( auto nameVarPair = mExternalNamesToVariables.begin(); nameVarPair != mExternalNamesToVariables.end(); ++nameVarPair )
                {
                    result.insert( nameVarPair->second );
                }
                return result;
            }
            
            /**
             * @return The string being the prefix of the external name of any internally declared (not parsed) variable.
             */
            std::string externalVarNamePrefix() const
            {
                return mExternalVarNamePrefix;
            }
    
            /**
             * @param _var The variable to get the name for.
             * @param _friendlyName A flag that indicates whether to print the given variables name with its 
             *                       internal representation (false) or with its dedicated name.
             * @return The name of the given variable.
             */
            std::string getVariableName( const Variable& _var, bool _friendlyName = true ) const
            {
                return VariablePool::getInstance().getName( _var, _friendlyName );
            }
            
            /**
             * Gets the variable by its name. Note that this is expensive and should only be used
             * for outputting reasons. In the actual implementations you should store the variables instead.
             * @param _varName The name of the variable to search for.
			 * @param _byFriendlyName If the name is the friendly name.
             * @return The found variable.
             */
            Variable getArithmeticVariableByName( const std::string& _varName, bool _byFriendlyName = false ) const
            {
                for( auto nameVarPair = mExternalNamesToVariables.begin(); nameVarPair != mExternalNamesToVariables.end(); ++nameVarPair )
                {
                    if( VariablePool::getInstance().getName( nameVarPair->second, _byFriendlyName ) == _varName )
                        return nameVarPair->second;
                }
                assert( false );
                return mExternalNamesToVariables.begin()->second;
            }
            
            /**
             * @return The number of Boolean variables which have been generated.
             */
            size_t numberOfBooleanVariables() const
            {
                return mBooleanVariables.size();
            }
            
            
            /**
             * @return The number of real variables which have been generated.
             */
            unsigned numberOfRealVariables() const
            {
                unsigned result = 0;
                for( auto var = mExternalNamesToVariables.begin(); var != mExternalNamesToVariables.end(); ++var )
                    if( var->second.getType() == VariableType::VT_REAL )
                        ++result;
                return result;
            }
            
            /**
             * @return The number of integer variables which have been generated.
             */
            unsigned numberOfIntVariables() const
            {
                unsigned result = 0;
                for( auto var = mExternalNamesToVariables.begin(); var != mExternalNamesToVariables.end(); ++var )
                    if( var->second.getType() == VariableType::VT_INT )
                        ++result;
                return result;
            }
               
            /**
             * @param _booleanName The Boolean variable name to check.
             * @return true, if the given Boolean variable name already exists. 
             */
            bool booleanExistsAlready( const std::string& _booleanName ) const
            {
                for( auto iter = mBooleanVariables.begin(); iter != mBooleanVariables.end(); ++iter )
                    if( _booleanName == VariablePool::getInstance().getName( *iter, true ) ) return true;
                return false;
            }
            
            /**
             * Creates an auxiliary integer valued variable.
             * @param _externalPrefix The prefix of the external name of the auxiliary variable to construct.
             * @return A pair of the internal name of the variable and the a variable as an expression.
             */
            Variable newAuxiliaryIntVariable( const std::string& _externalPrefix = "h_i" )
            {
                std::stringstream out;
                if( !mExternalPrefixInitialized ) initExternalPrefix();
                out << mExternalVarNamePrefix << _externalPrefix << mAuxiliaryIntVarCounter++;
                return newArithmeticVariable( out.str(), VariableType::VT_INT );
            }
            
            /**
             * Creates an auxiliary real valued variable.
             * @param _externalPrefix The prefix of the external name of the auxiliary variable to construct.
             * @return A pair of the internal name of the variable and the a variable as an expression.
             */
            Variable newAuxiliaryRealVariable( const std::string& _externalPrefix = "h_r" )
            {
                std::stringstream out;
                if( !mExternalPrefixInitialized ) initExternalPrefix();
                out << mExternalVarNamePrefix << _externalPrefix << "_" << mAuxiliaryRealVarCounter++;
                return newArithmeticVariable( out.str(), VariableType::VT_REAL );
            }
            
            /**
             * Resets the variable pool.
             * Note: Do not use it. It is only made for the Benchmax-Tool.
             */
            void clear();

			/**
			 * Creates an variable.
			 * @param _name The external name of the variable to construct.
			 * @param _domain The domain of the variable to construct.
			 * @param _parsed A special flag indicating whether this variable is constructed during parsing.
			 * @return A pair of the internal name of the variable and the variable as an expression.
			 */
			Variable newVariable( const std::string& _name, VariableType _domain, bool _parsed = false );

			Variable newUninterpretedVariable( const std::string& _name, bool _parsed = false );
			
            /**
             * Creates an arithmetic variable.
             * @param _name The external name of the variable to construct.
             * @param _domain The domain of the variable to construct.
             * @param _parsed A special flag indicating whether this variable is constructed during parsing.
             * @return A pair of the internal name of the variable and the variable as an expression.
             */
            Variable newArithmeticVariable( const std::string& _name, VariableType _domain, bool _parsed = false );
            
            /**
             * Creates a new Boolean variable.
             * @param _name The external name of the variable to construct.
             * @param _parsed A special flag indicating whether this variable is constructed during parsing.
             */
            Variable newBooleanVariable( const std::string& _name, bool _parsed = false );
            
            /**
             * Creates an auxiliary Boolean variable.
             * @param _externalPrefix The prefix of the external name of the auxiliary variable to construct.
             * @return The internal name of the variable.
             */
            Variable newAuxiliaryBooleanVariable( const std::string& _externalPrefix = "h_b" );
			
			Variable newAuxiliaryUninterpretedVariable( const std::string& _externalPrefix = "h_b" );
            
            /**
             * Initializes the prefix of the external variable names of internally declared (not parsed) variables.
             */
            void initExternalPrefix();
    };

     /**
      * @return A constant reference to the shared constraint pool.
      */
     const VariableNamePool& variablePool();

	/**
	 * Constructs a new variable of the given domain.
	 * @param _name The intended name of the variable.
	 * @param _domain The domain of the variable.
	 * @return The constructed variable.
	 */
	Variable newVariable( const std::string& _name, VariableType _domain, bool _parsed = false );

	/**
	 * Constructs a new real variable.
	 * @param _name The intended name of the real variable.
	 * @return The constructed real variable.
	 */
	Variable newRealVariable( const std::string& _name );

	/**
	 * Constructs a new arithmetic variable of the given domain.
	 * @param _name The intended name of the arithmetic variable.
	 * @param _domain The domain of the arithmetic variable.
	 * @param _parsed If the variable name was parsed.
	 * @return The constructed arithmetic variable.
	 */
	Variable newArithmeticVariable( const std::string& _name, VariableType _domain, bool _parsed = false );

	/**
	 * Constructs a new Boolean variable.
	 * @param _name The intended name of the variable.
	 * @param _parsed If the variable name was parsed.
	 * @return A pointer to the name of the constructed Boolean variable.
	 */
	Variable newBooleanVariable( const std::string& _name, bool _parsed = false );

	/**
	 * Generates a fresh real variable and returns its identifier.
	 * @return The fresh real variable.
	 */
	Variable newAuxiliaryIntVariable();

	/**
	 * Generates a fresh real variable and returns its identifier.
	 * @param _varName The dedicated name of the real variable.
	 * @return The fresh real variable.
	 */
	Variable newAuxiliaryIntVariable( const std::string& _varName );

	/**
	 * Generates a fresh real variable and returns its identifier.
	 * @return The fresh real variable.
	 */
	Variable newAuxiliaryRealVariable();

	/**
	 * Generates a fresh real variable and returns its identifier.
	 * @param _varName The dedicated name of the real variable.
	 * @return The fresh real variable.
	 */
	Variable newAuxiliaryRealVariable( const std::string& _varName );

	/**
	 * Generates a fresh Boolean variable and returns its identifier.
	 * @return The identifier of a fresh Boolean variable.
	 */
	Variable newAuxiliaryBooleanVariable();
	
	Variable newAuxiliaryUninterpretedVariable();

	/**
	 * Generates a fresh variable of the given type.
	 * @param type Variable type.
	 * @return The identifier of a fresh variable.
	 */
	 Variable newAuxiliaryVariable(VariableType type);
    
}    // namespace carl
