/**
 * @author Sebastian Junges
 * @ingroup constraints
 */
#include "CompareRelation.h"

#include <cassert>

namespace carl
{
bool relationIsStrict(CompareRelation r)
{
    return r == CompareRelation::LT || r == CompareRelation::GT || r == CompareRelation::NEQ;
}

std::string toString(const CompareRelation& cr)
{
    switch(cr)
    {
    case CompareRelation::EQ:
        return "=";
    case CompareRelation::GE:
        return ">=";
    case CompareRelation::GT:
        return ">";
    case CompareRelation::LE:
        return "<=";
    case CompareRelation::LT:
        return "<";
    case CompareRelation::NEQ:
        return "!=";
    }
    assert(false);
    return "??";
}

std::ostream& operator<<(std::ostream& os, CompareRelation cr)
{
    return os << toString(cr);
}

}
