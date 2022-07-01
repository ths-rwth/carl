#include "debug.h"

#include <boost/core/demangle.hpp>

#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#include <boost/stacktrace.hpp>

#include <memory>
#include <sstream>

#if !defined __VS
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#endif

namespace carl {

std::string demangle(const char* name) {
	return boost::core::demangle(name);
}

void printStacktrace() {
	std::cerr << boost::stacktrace::stacktrace();
}

std::string callingFunction() {
	std::stringstream ss;
	ss << boost::stacktrace::stacktrace(1,1);
	return ss.str();
}

#ifndef NDEBUG

std::string last_assertion_string;
int last_assertion_code = 23;

/**
 * Actual signal handler.
 */
#ifdef __VS
__declspec(noreturn) static void handle_signal(int signal) {
#else
[[noreturn]] static void handle_signal(int signal) {
#endif
	//printStacktrace(false);
	std::cerr << std::endl << "Catched SIGABRT " << signal << ", exiting with " << (last_assertion_code%256) << std::endl;
	if (!last_assertion_string.empty()) {
		std::cerr << "Last Assertion catched is: " << last_assertion_string << std::endl;
		std::cerr << "Please check if this is the assertion that is actually thrown." << std::endl;
	}
	exit(last_assertion_code % 256);
}
/**
 * Installs the signal handler.
 */
static bool install_signal_handler() noexcept {
	// CARL_LOG_INFO("carl.util", "Installing signal handler for SIGABRT");
	// std::cerr << "Installing signal handler for SIGABRT" << std::endl;
	std::signal(SIGABRT, handle_signal);
	return true;
}
/**
 * Static variable that ensures that install_signal_handler is called.
 */
static bool signal_installed = install_signal_handler();
#endif

}
