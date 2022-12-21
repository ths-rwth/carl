#include <benchmark/benchmark.h>


#include <carl-arith/ran/ran.h>

using Poly = carl::UnivariatePolynomial<mpq_class>;

class RF_Fixture: public benchmark::Fixture {	
};

BENCHMARK_F(RF_Fixture, Real_Roots_1)(benchmark::State& state) {
	carl::Variable x = carl::fresh_real_variable("x");
	Poly p = Poly(x, {-1, 12, -96, 384});
	// (384)*x^3 + (-96)*x^2 + (12)*x^1 + -1

	for (auto _ : state) {
		auto rans = carl::real_roots(p, carl::Interval<mpq_class>::unbounded_interval());
	}
}

BENCHMARK_F(RF_Fixture, Real_Roots_2)(benchmark::State& state) {
	carl::Variable x = carl::fresh_real_variable("x");
	Poly p = Poly(x, {-66864570625487788604261524190527488000000000000000000000000000000000000000000000000_mpq, 29854830731431634049898015056382132224000000000000000000000000000000000000000000000_mpq,
	-505416087363206530647584374267211415552000000000000000000000000000000000000000000_mpq, 4175633824067780996197090619870479908864000000000000000000000000000000000000000_mpq,
	-22123075889900509025587890607637359755264000000000000000000000000000000000000_mpq, 84105767440004379479396048598450604867584000000000000000000000000000000000_mpq,
	-243736416758800137976000425396107778981888000000000000000000000000000000_mpq, 557927276756885723844248187821195727470592000000000000000000000000000_mpq,
	-1028655240376238080386120074233535281496064000000000000000000000000_mpq,
	1537205882314544520383371309153190311624704000000000000000000000_mpq, -1850245296283620589500378012241541801705472000000000000000000_mpq,
	1759094245770532463939207746344508321431552000000000000000_mpq, -1274862311787402732031743433553883636424704000000000000_mpq,
	662971140168469203991742914981670449102848000000000_mpq, -221093370976852468477758191983112847898368000000_mpq,
	35982863100180637437263547995778211974592000_mpq, -222040198421806427031529186994264976123_mpq});
	// (-222040198421806427031529186994264976123)*skoX^16 + (35982863100180637437263547995778211974592000)*skoX^15 +
	// (-221093370976852468477758191983112847898368000000)*skoX^14 + (662971140168469203991742914981670449102848000000000)*skoX^13 +
	// (-1274862311787402732031743433553883636424704000000000000)*skoX^12 + (1759094245770532463939207746344508321431552000000000000000)*skoX^11 +
	// (-1850245296283620589500378012241541801705472000000000000000000)*skoX^10 + (1537205882314544520383371309153190311624704000000000000000000000)*skoX^9 +
	// (-1028655240376238080386120074233535281496064000000000000000000000000)*skoX^8 + (557927276756885723844248187821195727470592000000000000000000000000000)*skoX^7 +
	// (-243736416758800137976000425396107778981888000000000000000000000000000000)*skoX^6 + (84105767440004379479396048598450604867584000000000000000000000000000000000)*skoX^5 +
	// (-22123075889900509025587890607637359755264000000000000000000000000000000000000)*skoX^4 + (4175633824067780996197090619870479908864000000000000000000000000000000000000000)*skoX^3 +
	// (-505416087363206530647584374267211415552000000000000000000000000000000000000000000)*skoX^2 + (29854830731431634049898015056382132224000000000000000000000000000000000000000000000)*skoX^1 +
	// -66864570625487788604261524190527488000000000000000000000000000000000000000000000000

	for (auto _ : state) {
		auto rans = carl::real_roots(p, carl::Interval<mpq_class>::unbounded_interval());
	}
}



