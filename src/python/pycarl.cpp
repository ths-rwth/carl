//#include <python3.4m/Python.h>
#include <python2.7/Python.h>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/RationalFunction.h"
#include "carl/util/stringparser.h"

#include <gmpxx.h>
typedef mpq_class Rational;

//typedef double Rational;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;

#include "pycarl_parser.h"

namespace boost{

namespace python{
/*
template <class T>
inline T* get_pointer( std::shared_ptr<const T> const& p) {
	return const_cast<T*>(p.get());
}
*/
/*
template <class T>
struct pointee< std::shared_ptr<const T> > {
	typedef T type;
};
*/

namespace objects {

/**
 * This template instantiation hack allows the shared_ptr<const monomial> to work
 */
template <>
void* pointer_holder<std::shared_ptr<const carl::Monomial>, carl::Monomial>::holds(type_info dst_t, bool null_ptr_only)
{
    typedef typename boost::remove_const< carl::Monomial >::type non_const_value;

    if (dst_t == python::type_id<std::shared_ptr<const carl::Monomial>>()
        && !(null_ptr_only && get_pointer(this->m_p))
    )
        return &this->m_p;

    carl::Monomial* p0
#  if BOOST_WORKAROUND(__SUNPRO_CC, BOOST_TESTED_AT(0x590))
        = static_cast<carl::Monomial*>( get_pointer(this->m_p) )
#  else
        = const_cast<carl::Monomial*>(get_pointer(this->m_p))
#  endif
        ;
    non_const_value* p = const_cast<non_const_value*>( p0 );

    if (p == 0)
        return 0;

    if (void* wrapped = holds_wrapped(dst_t, p, p))
        return wrapped;

    type_info src_t = boost::python::type_id<non_const_value>();
    return src_t == dst_t ? p : find_dynamic_type(p, src_t, dst_t);
}

}

}}



/// @brief Type that allows for registration of conversions from
///        python iterable types.
struct iterable_converter
{
  /// @note Registers converter from a python interable type to the
  ///       provided type.
  template <typename Container>
  iterable_converter&
  from_python()
  {
    boost::python::converter::registry::push_back(
      &iterable_converter::convertible,
      &iterable_converter::construct<Container>,
      boost::python::type_id<Container>());

    // Support chaining.
    return *this;
  }

  /// @brief Check if PyObject is iterable.
  static void* convertible(PyObject* object)
  {
    return PyObject_GetIter(object) ? object : NULL;
  }

  /// @brief Convert iterable PyObject to C++ container type.
  ///
  /// Container Concept requirements:
  ///
  ///   * Container::value_type is CopyConstructable.
  ///   * Container can be constructed and populated with two iterators.
  ///     I.e. Container(begin, end)
  template <typename Container>
  static void construct(
    PyObject* object,
    boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    namespace python = boost::python;
    // Object is a borrowed reference, so create a handle indicating it is
    // borrowed for proper reference counting.
    python::handle<> handle(python::borrowed(object));

    // Obtain a handle to the memory block that the converter has allocated
    // for the C++ type.
    typedef boost::python::converter::rvalue_from_python_storage<Container>
                                                                storage_type;
    void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

    typedef boost::python::stl_input_iterator<typename Container::value_type>
                                                                    iterator;

    // Allocate the C++ type into the converter's memory block, and assign
    // its handle to the converter's convertible variable.  The C++
    // container is populated by passing the begin and end iterators of
    // the python object to the container's constructor.
    new (storage) Container(
      iterator(python::object(handle)), // begin
      iterator());                      // end
    data->convertible = storage;
  }
};


/// @brief Type that allows for registration of conversions from
///        python iterable types.
struct dict_converter {
	/// @note Registers converter from a python interable type to the
	///       provided type.
	template<typename Container>
	dict_converter&
	from_python() {
		boost::python::converter::registry::push_back(
				&dict_converter::convertible,
				&dict_converter::construct<Container>,
				boost::python::type_id<Container>());

		// Support chaining.
		return *this;
	}

	/// @brief Check if PyObject is dictionary.
	static void* convertible(PyObject* object) {
		return PyDict_Check(object) ? object : NULL;
	}

	/// @brief Convert iterable PyObject to C++ container type.
	///
	/// Container Concept requirements:
	///
	///   * Container::value_type is CopyConstructable.
	///   * Container can be constructed and populated with two iterators.
	///     I.e. Container(begin, end)
	template<typename Container>
	static void construct(PyObject* object,
			boost::python::converter::rvalue_from_python_stage1_data* data) {
		namespace python = boost::python;
		// Object is a borrowed reference, so create a handle indicating it is
		// borrowed for proper reference counting.
		python::handle<> handle(python::borrowed(object));

		// Obtain a handle to the memory block that the converter has allocated
		// for the C++ type.
		typedef boost::python::converter::rvalue_from_python_storage<Container> storage_type;
		void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

		typedef boost::python::stl_input_iterator<typename Container::value_type> iterator;

		// Allocate the C++ type into the converter's memory block, and assign
		// its handle to the converter's convertible variable.  The C++
		// container is populated by passing the begin and end iterators of
		// the python object to the container's constructor.
		auto container = new (storage) Container();
		data->convertible = storage;

		PyObject *key, *value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(object, &pos, &key, &value)) {
			/* do something interesting with the values... */
			auto keyValue = boost::python::extract<typename Container::key_type>(key)();
			auto mapValue = boost::python::extract<typename Container::mapped_type>(value)();

			container->emplace(keyValue, mapValue);
		}

	}
};

/** to-python convert from maps */
struct map_to_dict {
	template<typename Map>
	struct map_to_dict_impl {
		static PyObject* convert(const Map& map) {
			return boost::python::incref(make_dict(map).ptr());
		}
	private:
		static boost::python::dict make_dict(const Map& map) {
			auto d = boost::python::dict();
			for (auto& i : map) {
				d[i.first] = i.second;
			}
			return d;
		}
	};
	template<typename Map>
	void to_python() {
		boost::python::to_python_converter<Map, map_to_dict_impl<Map> >();
	}

};

struct iter_to_tuple {
	template<typename Iter>
	struct iter_to_set_impl {
		static PyObject* convert(const Iter& iter) {
			return boost::python::incref(make_tuple(iter).ptr());
		}
	private:
		static boost::python::tuple make_tuple(const Iter& iter) {
			boost::python::list l;
			for(auto& i : iter) {
				l.append(i);
			}
			return boost::python::tuple(l);
		}
	};
	template<typename Iter>
	void to_python() {
		boost::python::to_python_converter<Iter, iter_to_set_impl<Iter> >();
	}

};

struct rational_converter {
	  rational_converter()
	  {
	    boost::python::converter::registry::push_back(
	      &rational_converter::convertible,
	      &rational_converter::construct,
	      boost::python::type_id<Rational>());
	  }

	  /// @brief Check if PyObject is iterable.
	  static void* convertible(PyObject* object)
	  {
	    return PyFloat_Check(object) ? object : NULL;
	  }

	  static void construct(
	    PyObject* object,
	    boost::python::converter::rvalue_from_python_stage1_data* data)
	  {
	    namespace python = boost::python;
	    // Obtain a handle to the memory block that the converter has allocated
	    // for the C++ type.
	    typedef boost::python::converter::rvalue_from_python_storage<Rational>
	                                                                storage_type;
	    void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

	    new (storage) Rational(PyFloat_AsDouble(object));
	    data->convertible = storage;
	  }
};

std::string rational_str(const Rational& rat) {
	return rat.get_str();
}

BOOST_PYTHON_MODULE(libpycarl)
{
	using namespace boost::python;

	// Register interable conversions.
	iterable_converter()
		.from_python<std::vector<double> >()
		.from_python<std::list<double> >().from_python<std::vector<std::string> >()
		.from_python<std::list<std::string> >()
		;
	dict_converter()
		.from_python<std::map<carl::Variable, Rational> >()
		;

	map_to_dict()
		.to_python<std::map<std::string, carl::Variable> >()
		;
	iter_to_tuple()
		.to_python<std::set<carl::Variable> >()
		;
/*
	class_<std::map<std::string, Variable> >("VariableMap")
		.def(map_indexing_suite<std::map<std::string, Variable> >() );
*/

	class_<Rational>("Rational", init<double>())
		.def("__float__", &Rational::get_d)
		.def("__str__", &rational_str)
		;
	rational_converter();
	//boost::python::implicitly_convertible<Rational,double>();
	//boost::python::implicitly_convertible<double,Rational>();

	enum_<carl::VariableType>("VariableType")
		.value("BOOL", carl::VariableType::VT_BOOL)
		.value("INT", carl::VariableType::VT_INT)
		.value("REAL", carl::VariableType::VT_REAL);

	class_<carl::Variable>("Variable", no_init)//init<std::size_t, VariableType, std::size_t>())
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator*))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, const carl::Monomial::Arg&)>(&carl::operator*))
		.def(self_ns::str(self_ns::self))
		;
/*
	class_<carl::Monomial, carl::Monomial::Arg, boost::noncopyable>("Monomial", no_init)
		.def("tdeg", &carl::Monomial::tdeg)
		.def(self_ns::str(self_ns::self))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, const carl::Monomial::Arg&)>(&carl::operator*))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, carl::Variable::Arg)>(&carl::operator*))
		;
*/
	class_<carl::Term<Rational>>("Term")
		.def(init<Rational>())
		.def(init<carl::Variable::Arg>())
		.def(init<const carl::Monomial::Arg&>())
		.def(init<Rational, const carl::Monomial::Arg&>())
		.def(init<Rational, carl::Variable::Arg, carl::exponent>())
		.def(self_ns::str(self_ns::self))
		;

	class_<Polynomial>("Polynomial", init<Rational>())
		.def(init<carl::Variable::Arg>())
		.def(init<const carl::Monomial::Arg&>())
		.def("evaluate", &Polynomial::evaluate<Rational>)
		.def("gatherVariables", static_cast<std::set<carl::Variable> (Polynomial::*)() const>(&Polynomial::gatherVariables))
		.def(self_ns::str(self_ns::self))
		;

	class_<RationalFunction>("RationalFunction", init<Polynomial, Polynomial>())
		.def("evaluate", &RationalFunction::evaluate)
		.def("gatherVariables", static_cast<std::set<carl::Variable> (RationalFunction::*)() const>(&RationalFunction::gatherVariables))
		.def(self_ns::str(self_ns::self))
		;

	def("parsePolynomial", &parsePolynomial);
	def("parseRationalFunction", &parseRationalFunction);

	/*class_<carl::StringParser>("StringParser")
		.def("setVariables", &carl::StringParser::setVariables)
		.def("variables", &carl::StringParser::variables, return_value_policy<return_by_value>()) //capture map by value, to convert to dict()
		.def("parseMultivariatePolynomial", &carl::StringParser::parseMultivariatePolynomial<Rational>)
		.def("parseRationalFunction", &carl::StringParser::parseRationalFunction<Rational>)
		;*/

	class_<carl::VariablePool, boost::noncopyable>("VariablePoolInst", no_init)
		//.def("getInstance", &carl::VariablePool::getInstance, return_value_policy<reference_existing_object>())
		//.staticmethod("getInstance")
		.def("getFreshVariable", static_cast<carl::Variable (carl::VariablePool::*)(carl::VariableType)>(&carl::VariablePool::getFreshVariable))
		.def("getFreshVariable", static_cast<carl::Variable (carl::VariablePool::*)(const std::string&, carl::VariableType)>(&carl::VariablePool::getFreshVariable))
		;

	class_<carl::MonomialPool, boost::noncopyable>("MonomialPoolInst", no_init)
		//.def("getInstance", &carl::MonomialPool::getInstance, return_value_policy<reference_existing_object>())
		//.staticmethod("getInstance")
		.def("create", static_cast<carl::Monomial::Arg (carl::MonomialPool::*)(carl::Variable::Arg, carl::exponent)>(&carl::MonomialPool::create))
		;

	scope().attr("VariablePool") = object(ptr(&carl::VariablePool::getInstance()));
	scope().attr("MonomialPool") = object(ptr(&carl::MonomialPool::getInstance()));
}
