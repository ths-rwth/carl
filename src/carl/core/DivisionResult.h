/** 
 * @file   DivisionResult.h
 * @author Sebastian Junges
 *
 */

#pragma once

namespace carl {

/**
 * A strongly typed pair encoding the result of a division, 
 * being a quotient and a remainder.
 */
template<typename Type>
struct DivisionResult {
	Type quotient;
	Type remainder;
};
} // namespace carl