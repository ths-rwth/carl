/**
 * @file UEquality.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-20
 * @version 2014-10-22
 */

#include "UEquality.h"

namespace carl
{        
    bool UEquality::operator==( const UEquality& _ueq ) const
    {
        if( mNegated != _ueq.negated() )
            return false;
        if( lhsIsUV() != _ueq.lhsIsUV() )
        {
            return false;
        }
        if( rhsIsUV() != _ueq.rhsIsUV() )
        {
            return false;
        }
        if( lhsIsUV() )
        {
            if( rhsIsUV() )
            {
                return lhsAsUV() == _ueq.lhsAsUV() && rhsAsUV() == _ueq.rhsAsUV();
            }
            else
            {
                return lhsAsUV() == _ueq.lhsAsUV() && rhsAsUF() == _ueq.rhsAsUF();
            }
        }
        else
        {
            if( rhsIsUV() )
            {
                return lhsAsUF() == _ueq.lhsAsUF() && rhsAsUV() == _ueq.rhsAsUV();
            }
            else
            {
                return lhsAsUF() == _ueq.lhsAsUF() && rhsAsUF() == _ueq.rhsAsUF();
            }
        }
    }

    bool UEquality::operator<( const UEquality& _ueq ) const
    {
        if( !mNegated && _ueq.negated() )
            return true;
        if( mNegated && !_ueq.negated() )
            return false;
        if( lhsIsUV() && !_ueq.lhsIsUV() )
        {
            return true;
        }
        else if( !lhsIsUV() && _ueq.lhsIsUV() )
        {
            return false;
        }
        if( rhsIsUV() && !_ueq.rhsIsUV() )
        {
            return true;
        }
        else if( !rhsIsUV() && _ueq.rhsIsUV() )
        {
            return false;
        }
        assert( lhsIsUV() == _ueq.lhsIsUV() );
        if( lhsIsUV() )
        {
            if( lhsAsUV() < _ueq.lhsAsUV() )
            {
                return true;
            }
            else if( _ueq.lhsAsUV() < lhsAsUV() )
            {
                return false;
            }
        }
        else
        {
            if( lhsAsUF() < _ueq.lhsAsUF() )
            {
                return true;
            }
            else if( _ueq.lhsAsUF() < lhsAsUF() )
            {
                return false;
            }
        }
        assert( (lhsIsUV() && lhsAsUV() == _ueq.lhsAsUV()) || (lhsIsUF() && lhsAsUF() == _ueq.lhsAsUF()) );
        if( rhsIsUV() )
        {
            return rhsAsUV() < _ueq.rhsAsUV();
        }
        else
        {
            return rhsAsUF() < _ueq.rhsAsUF();
        }
        return false;
    }

    std::string UEquality::toString( bool _infix, bool _friendlyNames ) const
    {
        std::string result = "";
        if( !_infix )
        {
            if( negated() )
                result += "(!= ";
            else
                result += "(= ";
        }
        if( lhsIsUV() )
        {
            result += lhsAsUV().toString( _friendlyNames );
        }
        else
        {
            result += lhsAsUF().toString( _infix, _friendlyNames );
        }
        if( _infix )
            result += (negated() ? "!=" : "=");
        else
            result += " ";
        if( rhsIsUV() )
        {
            result += rhsAsUV().toString( _friendlyNames );
        }
        else
        {
            result += rhsAsUF().toString( _infix, _friendlyNames );
        }
        if( !_infix )
        {
            result += ")";
        }
        return result;
    }

    std::ostream& operator<<( std::ostream& _os, const UEquality& _ueq )
    {
        if( _ueq.negated() )
            _os << "(!= ";
        else
            _os << "(= ";
        if( _ueq.lhsIsUV() )
        {
            _os << _ueq.lhsAsUV();
        }
        else
        {
            _os << _ueq.lhsAsUF();
        }
        _os << " ";
        if( _ueq.rhsIsUV() )
        {
            _os << _ueq.rhsAsUV();
        }
        else
        {
            _os << _ueq.rhsAsUF();
        }
        _os << ")";
        return _os;
    }
} // end namespace carl