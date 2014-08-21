/**
 * @file CADLogging.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../core/logging.h"

namespace carl {
namespace cad {

template<typename Coeff>
class DotGenerator: public carl::Singleton<DotGenerator<Coeff>> {
	struct DotFormatter: public carl::logging::Formatter {
		virtual void prefix(std::ostream&, const carl::logging::Timer&, const std::string&, carl::logging::LogLevel, const carl::logging::RecordInfo&) {
		}
	};
	struct DotSink : public carl::logging::FileSink {
		DotSink(const std::string& filename) : FileSink(filename) {
			this->os << "strict digraph {" << std::endl;
			this->os << "graph [rankdir=\"LR\"];" << std::endl;
			this->os << "graph [splines=line];" << std::endl;
		}
		~DotSink() {
			this->os << "}" << std::endl;
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
	DotGenerator(): carl::Singleton<DotGenerator<Coeff>>() {
		this->configure("lifting");
		this->configure("elimination");
	}
	
	void log(const std::string& id, const std::string& s) {
		LOGMSG_TRACE("carl.cad.dot." + id, s);
	}
	
	void node(const std::string& id, const std::string& name, const std::string& options) {
		LOGMSG_TRACE("carl.cad.dot." + id, "\"" << name << "\" [" << options << "];");
	}
	void node(const std::string& id, const carl::cad::UPolynomial<Coeff>* name, const std::string& options) {
		if (name->isZero()) return;
		LOGMSG_TRACE("carl.cad.dot." + id, "\"" << name->mainVar() << ":" << *name << "\" [" << options << "];");
	}

	void edge(const std::string& id, const carl::cad::UPolynomial<Coeff>* from, const carl::cad::UPolynomial<Coeff>* to, const std::string& options = "") {
		edge(id, *from, *to, options);
	}
	void edge(const std::string& id, const carl::cad::UPolynomial<Coeff>* from, const carl::cad::UPolynomial<Coeff>& to, const std::string& options = "") {
		edge(id, *from, to, options);
	}
	void edge(const std::string& id, const carl::cad::UPolynomial<Coeff>& from, const carl::cad::UPolynomial<Coeff>& to, const std::string& options = "") {
		if (to.isZero()) return;
		LOGMSG_TRACE("carl.cad.dot." + id, "\"" << from.mainVar() << ":" << from << "\" -> \"" << to.mainVar() << ":" << to << "\" [" << options << "];");
	}
	void hyperedge(const std::string& id, const std::initializer_list<const carl::cad::UPolynomial<Coeff>*>& from, const carl::cad::UPolynomial<Coeff>* to) {
		hyperedge(id, from, *to);
	}
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
			LOGMSG_TRACE("carl.cad.dot." + id, "\"" << name.str() << "\" [shape=point];");
			for (auto f: from) {
				LOGMSG_TRACE("carl.cad.dot." + id, "\"" << f->mainVar() << ":" << *f << "\" -> \""<< name.str() << "\";");
			}
			LOGMSG_TRACE("carl.cad.dot." + id, "\"" << name.str() << "\" -> \"" << to.mainVar() << ":" << to << "\";");
		}
	}
	
	void sample(const std::string& id, const carl::RealAlgebraicNumberPtr<Coeff>& s, const carl::RealAlgebraicNumberPtr<Coeff>& p) {
		LOGMSG_TRACE("carl.cad.dot." + id, "\"" << p << "\" -> \"" << s << "\";");
	}
};

#ifdef LOGGING_CARL

#define DOT_NODE(id, name, options) carl::cad::DotGenerator<Coefficient>::getInstance().node(id, name, options)
#define DOT_EDGE(id, from, to, options) carl::cad::DotGenerator<Coefficient>::getInstance().edge(id, from, to, options)
#define DOT_HYPEREDGE(id, from, to) carl::cad::DotGenerator<Coefficient>::getInstance().hyperedge(id, from, to)

#else

#define DOT_NODE(id, name, options)
#define DOT_EDGE(id, from, to, options)
#define DOT_HYPEREDGE(id, from, to)

#endif

}
}