
/**
 * @file CADLogging.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../core/logging.h"
#include "../util/Timer.h"

namespace carl {
namespace cad {

#if defined(LOGGING)

/**
 * This class is used to create a dot file representing the elimination phase of the CAD.
 */
class DotGenerator: public carl::Singleton<DotGenerator> {
	struct DotFormatter: public carl::logging::Formatter {
		virtual void prefix(std::ostream&, const carl::Timer&, const std::string&, carl::logging::LogLevel, const carl::logging::RecordInfo&) {
		}
	};
	struct DotSink : public carl::logging::FileSink {
		DotSink(const std::string& filename) : FileSink(filename) {
			log() << "strict digraph {" << std::endl;
			log() << "graph [rankdir=\"LR\"];" << std::endl;
			log() << "graph [splines=line];" << std::endl;
		}
		~DotSink() {
			log() << "}" << std::endl;
		}
	};

	void configure(const std::string& id) {
		carl::logging::logger().configure("carl.cad.dot." + id, std::make_shared<DotSink>("carl.cad.dot." + id));
		carl::logging::logger().formatter("carl.cad.dot." + id, std::make_shared<DotFormatter>());
		carl::logging::logger().filter("carl.cad.dot." + id)
			("", carl::logging::LogLevel::LVL_OFF)
			("carl.cad.dot." + id, carl::logging::LogLevel::LVL_ALL)
		;
	}
	
public:
	DotGenerator(): carl::Singleton<DotGenerator>() {
		this->configure("lifting");
		this->configure("elimination");
	}
	
	void log(const std::string& id, const std::string& s) {
		CARL_LOG_TRACE("carl.cad.dot." + id, s);
	}
	
	void node(const std::string& id, const std::string& name, const std::string& options) {
		CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << name << "\" [" << options << "];");
	}
	template<typename Coeff>
	void node(const std::string& id, const carl::cad::UPolynomial<Coeff>* name, const std::string& options) {
		if (name->isZero()) return;
		CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << name->mainVar() << ":" << *name << "\" [" << options << "];");
	}

	template<typename Coeff>
	void edge(const std::string& id, const carl::cad::UPolynomial<Coeff>* from, const carl::cad::UPolynomial<Coeff>* to, const std::string& options = "") {
		edge(id, *from, *to, options);
	}
	template<typename Coeff>
	void edge(const std::string& id, const carl::cad::UPolynomial<Coeff>* from, const carl::cad::UPolynomial<Coeff>& to, const std::string& options = "") {
		edge(id, *from, to, options);
	}
	template<typename Coeff>
	void edge(const std::string& id, const carl::cad::UPolynomial<Coeff>& from, const carl::cad::UPolynomial<Coeff>& to, const std::string& options = "") {
		if (to.isZero()) return;
		CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << from.mainVar() << ":" << from << "\" -> \"" << to.mainVar() << ":" << to << "\" [" << options << "];");
	}
	template<typename Coeff>
	void hyperedge(const std::string& id, const std::initializer_list<const carl::cad::UPolynomial<Coeff>*>& from, const carl::cad::UPolynomial<Coeff>* to) {
		hyperedge(id, from, *to);
	}
	template<typename Coeff>
	void hyperedge(const std::string& id, const std::initializer_list<const carl::cad::UPolynomial<Coeff>*>& from, const carl::cad::UPolynomial<Coeff>& to) {
		if (to.isZero()) return;
		assert(from.size() > 0);
		bool same = true;
		auto first = *from.begin();
		for (auto t: from) if (t != first) same = false;
		if (same) {
			edge(id, first, to, "label=\"hyperedge\"");
		} else {
			std::stringstream name;
			std::vector<const carl::cad::UPolynomial<Coeff>*> hashes(from.begin(), from.end());
			std::sort(hashes.begin(), hashes.end());
			for (auto h: hashes) name << h;
			CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << name.str() << "\" [shape=point];");
			for (auto f: from) {
				CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << f->mainVar() << ":" << *f << "\" -> \""<< name.str() << "\";");
			}
			CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << name.str() << "\" -> \"" << to.mainVar() << ":" << to << "\";");
		}
	}

	template<typename Coeff>
	void sample(const std::string& id, const carl::RealAlgebraicNumber<Coeff>& s, const carl::RealAlgebraicNumber<Coeff>& p) {
		CARL_LOG_TRACE("carl.cad.dot." + id, "\"" << p << "\" -> \"" << s << "\";");
	}
};

#define DOT_NODE(id, name, options) carl::cad::DotGenerator::getInstance().node(id, name, options)
#define DOT_EDGE(id, from, to, options) carl::cad::DotGenerator::getInstance().edge(id, from, to, options)
#define DOT_HYPEREDGE(id, from, to) carl::cad::DotGenerator::getInstance().hyperedge(id, from, to)

#else

#define DOT_NODE(id, name, options)
#define DOT_EDGE(id, from, to, options)
#define DOT_HYPEREDGE(id, from, to)

#endif

}
}
