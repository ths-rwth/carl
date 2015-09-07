#pragma once

#include <iostream>
#include <vector>

#include "UnivariatePolynomial.h"

namespace carl {

template<typename Coeff>
class PolyMatrix {
private:
	std::size_t dim;
	Variable var;
	std::vector<std::vector<Coeff>> data;
public:
	PolyMatrix(std::size_t dim, Variable::Arg var): dim(dim), var(var), data(dim) {
		for (std::size_t i = 0; i < dim; i++) {
			data[i].resize(dim, Coeff(0));
		}
	}
	void set(std::size_t row, std::size_t indent, const UnivariatePolynomial<Coeff>& p) {
		assert(p.mainVar() == var);
		for (std::size_t i = 0; i <= p.degree(); i++) {
			data[row][dim - indent - i - 1] = p.coefficients()[i];
		}
	}
	void eliminate(std::size_t from, std::size_t to, std::size_t col) {
		Coeff f = data[from][col];
		Coeff t = data[to][col];
		for (std::size_t i = 0; i < dim; i++) {
			data[from][i] *= t;
			if (i == col) data[to][i] = Coeff(0);
			else data[to][i] = data[to][i] * f - data[from][i];
		}
	}
	template<typename C>
	friend std::ostream& operator<<(std::ostream& os, const PolyMatrix<C>& pm) {
		for (std::size_t i = 0; i < pm.dim; i++) {
			for (std::size_t j = 0; j < pm.dim; j++) {
				if (!pm.data[i][j].isZero()) os << pm.data[i][j];
				os << "\t";
			}
			os << std::endl;
		}
		return os;
	}
};

class Resultant {
private:
	/**
	 * Applies preprocessing to p and q and tries to calculate the resultant for easy cases.
	 * If the resultant could be obtained, res will hold the resultant and true is returned.
	 */
	template<typename Coeff>
	bool preprocessing(UnivariatePolynomial<Coeff>& p, UnivariatePolynomial<Coeff>& q, UnivariatePolynomial<Coeff>& res) const {
		if (p.isZero() || q.isZero()) {
			res = UnivariatePolynomial<Coeff>(q.mainVar());
			return true;
		}
		p = p.coprimeCoefficients();
		q = q.coprimeCoefficients();

		if (p.isConstant()) {
			res = p;
			return true;
		}
		if (q.isConstant()) {
			res = q;
			return true;
		}

		if (p.isLinearInMainVar() && q.isLinearInMainVar() ) {
			Coeff r = p.lcoeff() * q.tcoeff() - p.tcoeff() * q.lcoeff();
			res = UnivariatePolynomial<Coeff>(q.mainVar(), r);
			return true;
		}
		return false;
	}
public:

	template<typename Coeff>
	UnivariatePolynomial<Coeff> resultant_z3(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q
	) const {
		//std::cout << "resultant(" << p << ", " << q << ", " << q.mainVar() << ")" << std::endl;
		if (p.isZero() || q.isZero()) {
			//std::cout << "Zero" << std::endl;
			return UnivariatePolynomial<Coeff>(q.mainVar());
		}

		if (p.isConstant()) {
			//std::cout << "A is const" << std::endl;
			if (q.isConstant()) {
				return UnivariatePolynomial<Coeff>(q.mainVar(), Coeff(1));
			} else {
				return p.pow(q.degree());
			}
		}
		if (q.isConstant()) {
			//std::cout << "B is const" << std::endl;
		   return q.pow(q.degree());
		}

		UnivariatePolynomial<Coeff> nA(q.normalized());
		UnivariatePolynomial<Coeff> nB(p.normalized());

		//std::cout << "Content" << std::endl;
		Coeff cA = nA.content();
		Coeff cB = nB.content();
		Coeff A(nA.primitivePart());
		Coeff B(nB.primitivePart());
		//std::cout << "Done" << std::endl;
		cA = carl::pow(cA, p.degree());
		cB = carl::pow(cB, q.degree());
		Coeff t = cA * cB;

		//std::cout << "Pre" << std::endl;
		int s = 1;
		std::size_t degA = A.degree(q.mainVar());
		std::size_t degB = B.degree(q.mainVar());
		if (degA < degB) {
			std::swap(A, B);
			if (degA % 2 == 1 && degB % 2 == 1) s = -1;
		}

		Coeff R;
		Coeff g(1);
		Coeff h(1);
		Coeff new_h;
		bool div_res = false;

		while (true) {
			//std::cout << "Loop " << A << ", " << B << std::endl;
			//std::cout << "A = " << A << ", B = " << B << std::endl;
			degA = A.degree(q.mainVar());
			degB = B.degree(q.mainVar());
			//std::cout << "Degrees: " << degA << " / " << degB << std::endl;
			assert(degA >= degB);
			std::size_t delta = degA - degB;
			//std::cout << "1: delta = " << delta << std::endl;
			if (degA % 2 == 1 && degB % 2 == 1) s = -s;
			R = A.prem(B, q.mainVar());
			A = B;
			//std::cout << "R = " << R << std::endl;
			//std::cout << "g = " << g << std::endl;
			div_res = R.divideBy(g, B);
			assert(div_res);
			for (std::size_t i = 0; i < delta; i++) {
				//std::cout << "i = " << i << std::endl;
				div_res = B.divideBy(h, B);
				assert(div_res);
			}
			g = A.lcoeff(q.mainVar());
			//std::cout << "2: delta = " << delta << std::endl;
			//std::cout << "g = " << g << std::endl;
			new_h = carl::pow(g, delta);
			//std::cout << "Pow done" << delta << std::endl;
			if (delta > 1) {
				for (std::size_t i = 0; i < delta - 1; i++) {
					div_res = new_h.divideBy(h, new_h);
					assert(div_res);
				}
			}
			h = new_h;
			if (B.degree(q.mainVar()) == 0) {
				std::size_t degA = A.degree(q.mainVar());
				new_h = B.lcoeff(q.mainVar());
				new_h = carl::pow(new_h, degA);
				if (degA > 1) {
					for (std::size_t i = 0; i < degA - 1; i++) {
						div_res = new_h.divideBy(h, new_h);
						assert(div_res);
					}
				}
				h = new_h;
				if (s < 0) return UnivariatePolynomial<Coeff>(q.mainVar(), -t * h);
				return UnivariatePolynomial<Coeff>(q.mainVar(), t * h);
			}
		}
	}

	template<typename Coeff>
	void printMatrix(std::size_t width, const UnivariatePolynomial<Coeff>& p, std::size_t indent) const {
		for (std::size_t i = 0; i < width - p.degree() - indent; i++) std::cout << "\t";
		for (std::size_t i = p.degree()+1; i > 0; i--) {
			std::cout << p.coefficients()[i-1] << "\t";
		}
		std::cout << std::endl;
	}

	/**
	 * Eliminates the leading factor of p with q.
	 */
	template<typename Coeff>
	UnivariatePolynomial<Coeff> eliminate(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q
	) const {
		//std::cout << "eliminate " << p << " with " << q << std::endl;
		assert(p.mainVar() == q.mainVar());
		assert(p.degree() >= q.degree());
		std::size_t degdiff = p.degree() - q.degree();
		if (degdiff == 0) return p * q.lcoeff() - q * p.lcoeff();
		else return p * q.lcoeff() - q * p.lcoeff() * UnivariatePolynomial<Coeff>(p.mainVar(), Coeff(1), degdiff);
	}

	template<typename Coeff>
	UnivariatePolynomial<Coeff> resultant_det(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q
	) const {
		if (p.isZero() || q.isZero()) {
			return UnivariatePolynomial<Coeff>(q.mainVar());
		}
		if (p.isConstant()) {
			if (q.isConstant()) {
				return UnivariatePolynomial<Coeff>(p.mainVar(), Coeff(1));
			} else {
				return p.pow(q.degree());
			}
		}
		if (q.isConstant()) {
		   return q.pow(p.degree());
		}
		if (p == q) return UnivariatePolynomial<Coeff>(p.mainVar());

		UnivariatePolynomial<Coeff> f = q;
		UnivariatePolynomial<Coeff> g = p;
		if (f.degree() > g.degree()) std::swap(f, g);

		// Three stages:
		//   1. Eliminate leading coefficients of all g-rows with f at once
		//      -> Until last g-row can be eliminated with last f-row
		//   2. Eliminate leading coefficients of g-rows with f while still possible
		//      -> Until no g-row can be eliminated with last f-row.
		//         Now, there is a deg(f)-square-matrix at the lower right.
		//   3. Eliminate the square matrix at the lower right.

		// Sylvester Matrix of f and g:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//   g3  g2  g1  g0
		//       g3  g2  g1  g0
		//
		// First we eliminate g3 with f, resulting in:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//       g2' g1' g0
		//           g2' g1' g0
		//
		// Now we eliminate g2', result looks like this:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//           g1* g0*
		//               g1* g0*
		//
		// And finally, we eliminate g3' from the first g-row:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//               g0' t
		//               g1*  g0*
		//
		// Now, the first degree(f) columns are diagonal.
		//std::cout << "f = " << f << std::endl;
		//std::cout << "g = " << g << std::endl;
		UnivariatePolynomial<Coeff> gRest = g;
		// Do the elimination that works the same for all g-rows
		for (std::size_t i = 0; i < g.degree() - f.degree() + 1; i++) {
			//std::cout << gRest << " % " << f << std::endl;
			gRest = eliminate(gRest, f);
			//std::cout << "-> " << gRest << std::endl;
		}
		// Store result of first eliminations in matrix.
		//PolyMatrix<Coeff> matrix(f.degree() + g.degree(), f.mainVar());
		for (std::size_t i = 0; i < g.degree(); i++) {
			// Store f-rows
			//matrix.set(i, g.degree()-i-1, f);
		}
		// Finish eliminations of g-rows.
		std::vector<UnivariatePolynomial<Coeff>> m;
		m.resize(f.degree(), UnivariatePolynomial<Coeff>(f.mainVar()));
		//matrix.set(g.degree() + f.degree()-1, 0, gRest);
		m[f.degree()-1] = gRest;
		for (std::size_t i = 1; i < f.degree(); i++) {
			gRest = eliminate(gRest * g.mainVar(), f);
			//matrix.set(g.degree() + f.degree()-1-i, 0, gRest);
			m[f.degree()-1-i] = gRest;
		}
		//std::cout << m << std::endl;
		for (std::size_t i = 0; i < m.size()-1; i++) {
			for (std::size_t j = i+1; j < m.size(); j++) {
				m[j] = eliminate(m[j], m[i]);
			}
		}
		//std::cout << m << std::endl;

		Coeff determinant = f.lcoeff();//.pow(f.degree());
		//std::cout << "det = " << determinant << std::endl;
		for (std::size_t i = 0; i < m.size(); i++) {
			//std::cout << "   *= " << m[i].coefficients()[m.size()-1-i] << std::endl;
			if (m[i].degree() >= m.size()-1-i) {
				determinant *= m[i].coefficients()[m.size()-1-i];
			} else {
				determinant = Coeff(0);
			}
		}
		//determinant = m.back().tcoeff();
		determinant = determinant.coprimeCoefficients();
		//std::cout << "det = " << determinant << std::endl;

		return UnivariatePolynomial<Coeff>(f.mainVar(), determinant);
	}
};

}
