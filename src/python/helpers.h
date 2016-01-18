#pragma once
#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#include "carl/core/Monomial.h"


namespace boost{
    namespace python{
        namespace objects {

/**
 * This template specialization hack allows the shared_ptr<const Monomial> to work
 * by liberally throwing around const_cast
 */
            template<>
            void* pointer_holder<std::shared_ptr<const carl::Monomial>, carl::Monomial>::holds(
                    type_info dst_t, bool null_ptr_only) {
                typedef typename boost::remove_const<carl::Monomial>::type non_const_value;

                if (dst_t == python::type_id<std::shared_ptr<const carl::Monomial>>()
                    && !(null_ptr_only && get_pointer(this->m_p)))
                    return &this->m_p;

                carl::Monomial* p0
#  if BOOST_WORKAROUND(__SUNPRO_CC, BOOST_TESTED_AT(0x590))
                        = static_cast<carl::Monomial*>( get_pointer(this->m_p) )
#  else
                        = const_cast<carl::Monomial*>(get_pointer(this->m_p))
#  endif
                ;
                non_const_value* p = const_cast<non_const_value*>(p0);

                if (p == 0)
                    return 0;

                if (void* wrapped = holds_wrapped(dst_t, p, p))
                    return wrapped;

                type_info src_t = boost::python::type_id<non_const_value>();
                return src_t == dst_t ? p : find_dynamic_type(p, src_t, dst_t);
            }

        }}}

/**
 * Converts python iterables to STL containers
 */
struct iterable_converter
{
    /// @note Registers converter from a python interable type to the
    ///       provided type.
    template<typename Container>
    iterable_converter&
    from_python() {
        boost::python::converter::registry::push_back(
                &iterable_converter::convertible,
                &iterable_converter::construct<Container>,
                boost::python::type_id<Container>());

        // Support chaining.
        return *this;
    }

    /// @brief Check if PyObject is iterable.
    static void* convertible(PyObject* object) {
        return PyObject_GetIter(object) ? object : NULL;
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
        new (storage) Container(iterator(python::object(handle)), // begin
                                iterator());                      // end
        data->convertible = storage;
    }
};

/**
 * Converts python dictionaries to STL maps
 */
struct dict_converter {
    template<typename Container>
    dict_converter&
    from_python() {
        boost::python::converter::registry::push_back(
                &dict_converter::convertible,
                &dict_converter::construct<Container>,
                boost::python::type_id<Container>());

        return *this;
    }

    static void* convertible(PyObject* object) {
        return PyDict_Check(object) ? object : NULL;
    }

    template<typename Container>
    static void construct(PyObject* object,
                          boost::python::converter::rvalue_from_python_stage1_data* data) {
        namespace python = boost::python;
        python::handle<> handle(python::borrowed(object));

        typedef boost::python::converter::rvalue_from_python_storage<Container> storage_type;
        void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

        auto container = new (storage) Container();
        data->convertible = storage;

        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(object, &pos, &key, &value)) {
            auto keyValue =
                    boost::python::extract<typename Container::key_type>(key)();
            auto mapValue = boost::python::extract<
                    typename Container::mapped_type>(value)();
            container->emplace(keyValue, mapValue);
        }
    }
};

/**
 * Converts STL maps to python dictionaries
 */
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

/**
 * Converts STL iterables into python tuples
 */
struct iter_to_tuple {
    template<typename Iter>
    struct iter_to_set_impl {
        static PyObject* convert(const Iter& iter) {
            return boost::python::incref(make_tuple(iter).ptr());
        }
    private:
        static boost::python::tuple make_tuple(const Iter& iter) {
            boost::python::list l;
            for (auto& i : iter) {
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

/**
 * Converts python floats into rationals
 */
struct rational_converter {
    rational_converter() {
        boost::python::converter::registry::push_back(
                &rational_converter::convertible,
                &rational_converter::construct,
                boost::python::type_id<Rational>());
    }

    static void* convertible(PyObject* object) {
        return PyFloat_Check(object) ? object : NULL;
    }

    static void construct(PyObject* object,
                          boost::python::converter::rvalue_from_python_stage1_data* data) {
        namespace python = boost::python;
        typedef boost::python::converter::rvalue_from_python_storage<Rational> storage_type;
        void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

        new (storage) Rational(PyFloat_AsDouble(object));
        data->convertible = storage;
    }
};


#endif //CARL_HELPERS_H
