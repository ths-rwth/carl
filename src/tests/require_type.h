#pragma once

/// Helper to work with boost::concept_check.
template<typename T, typename TT>
void require_type(const TT& tt) {
	T t = tt;
	(void)(t);
}