
namespace carl {

    template<>
    inline mpq_class convert<double, mpq_class>(const double& n) {
    	return carl::rationalize<mpq_class>(n);
    }

    template<>
    inline double convert<mpq_class, double>(const mpq_class& n) {
    	return carl::toDouble(n);
    }

    template<>
    inline FLOAT_T<mpq_class> convert<double, FLOAT_T<mpq_class>>(const double& n) {
    	return FLOAT_T<mpq_class>(carl::rationalize<mpq_class>(n));
    }

    template<>
    inline double convert<FLOAT_T<mpq_class>, double>(const FLOAT_T<mpq_class>& n) {
    	return carl::toDouble(n.value());
    }

    template<>
    inline double convert<FLOAT_T<double>, double>(const FLOAT_T<double>& n) {
    	return n.value();
    }

    template<>
    inline FLOAT_T<double> convert<double, FLOAT_T<double>>(const double& n) {
    	return FLOAT_T<double>(n);
    }

    #ifdef USE_MPFR_FLOAT
    template<>
    inline double convert<FLOAT_T<mpfr_t>, double>(const FLOAT_T<mpfr_t>& n) {
    	return carl::toDouble(n);
    }

    template<>
    inline FLOAT_T<mpfr_t> convert<double, FLOAT_T<mpfr_t>>(const double& n) {
    	return FLOAT_T<mpfr_t>(n);
    }
    #endif

    #ifdef USE_CLN_NUMBERS
    template<>
    inline double convert<cln::cl_RA, double>(const cln::cl_RA& n) {
    	return carl::toDouble(n);
    }

	template<>
    inline cln::cl_RA convert<double, cln::cl_RA>(const double& n) {
    	return carl::rationalize<cln::cl_RA>(n);
    }

    template<>
    inline double convert<FLOAT_T<cln::cl_RA>, double>(const FLOAT_T<cln::cl_RA>& n) {
    	return carl::toDouble(n);
    }

    template<>
    inline FLOAT_T<cln::cl_RA> convert<double, FLOAT_T<cln::cl_RA>>(const double& n) {
    	return FLOAT_T<cln::cl_RA>(n);
    }

    #endif

} // namespace carl