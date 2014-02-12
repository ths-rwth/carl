#include "debug.h"

#include "platform.h"

#if defined(__CLANG) || defined(__GCC)
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <stdlib.h>
#include <unistd.h>
#endif
#include <cassert>
#include <map>
#include <sstream>

namespace carl {

#if defined(__CLANG) || defined(__GCC)
std::string Stacktrace::demangle(const void* stackframe, const char* sym) const {
	Dl_info info;
	std::string res = sym;
	if (dladdr(stackframe, &info) && info.dli_sname) {
		char* demangled = nullptr;
		int status = -1;
		if (info.dli_sname[0] == '_') {
			demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
		}
		if (status == 0) res = demangled;
		else if (info.dli_sname != 0) res = info.dli_sname;
		free(demangled);
	}
	return res;
}
#else
std::string Stacktrace::demangle(const void*, const char* sym) const {
	std::string res = sym;
	return res;
}
#endif

void Stacktrace::storeTrace(const std::string& name) {
	void* stack[128];
	const long unsigned maxFrames = sizeof(stack) / sizeof(void*);
	unsigned frames = (unsigned)backtrace(stack, maxFrames);
	// If stack frames are omitted, merge() will screw up. More than 100 stack frames should not happen anyway.
	assert(frames < maxFrames);
	std::vector<std::string> trace;
	if (frames == 0) {
		trace.push_back("\t<empty, probably corrupt>");
		this->traces[name] = trace;
		return;
	}
	trace.resize((unsigned)frames-1);
	char** symbols = backtrace_symbols(stack, (int)frames);
	
	for (unsigned i = 1; i < frames; i++) {
		std::stringstream ss;
		ss << this->col["red"] << "#" << (frames - i) << this->col["reset"] << "\t";
		ss << this->demangle(stack[i], symbols[i]) << this->col["gray"] << " @ " << stack[i] << this->col["reset"];
		trace[trace.size()-i] = ss.str();
	}
	this->traces[name] = trace;
}

std::ostream& Stacktrace::all(std::ostream& os) const {
	for (auto trace: this->traces) {
		os << this->col.at("red") << "Stacktrace " << this->col.at("green") << trace.first << this->col.at("reset") << std::endl;
		for (unsigned i = 0; i < trace.second.size(); i++) {
			os << trace.second[i] << std::endl;
		}
		os << std::endl;
	}
	return os;
}

std::ostream& Stacktrace::merged(std::ostream& os) const {
	if (this->traces.size() == 0) return os;
	
	// Process all stack frames at once until they differ
	unsigned firstDifferentFrame = 0;
	bool common = true;
	while (common) {
		std::string frame;
		bool first = true;
		for (auto trace: this->traces) {
			// Inspect all traces.
			if (trace.second.size() <= firstDifferentFrame) {
				// One has already finished, stop here.
				common = false;
				break;
			}
			if (first) { 
				// Take frame from first trace
				frame = trace.second[firstDifferentFrame];
				first = false;
			} else if (frame != trace.second[firstDifferentFrame]) {
				// Frame differs from previous, stop here.
				common = false;
				break;
			}
		}
		if (common) {
			// This frame was the same, print it.
			firstDifferentFrame++;
		}
	}
	
	// Now print remaining parts of all traces
	for (auto trace: this->traces) {
		os << this->col.at("gray") << "Remaining part of " << this->col.at("green") << trace.first << this->col.at("reset") << std::endl;
		if (firstDifferentFrame >= trace.second.size()) {
			os << this->col.at("red") << "Trace has no further frames" << this->col.at("reset") << std::endl;
		}
		for (int i = (int)trace.second.size()-1; i >= (int)firstDifferentFrame; i--) {
			os << trace.second[(unsigned)i] << std::endl;
		}
		os << std::endl;
	}
	
	// Finally print common stackframes
	os << this->col.at("gray") << "Common stackframes" << this->col.at("reset") << std::endl;
	for (int i = (int)firstDifferentFrame-1; i >= 0; i--) {
		os << this->traces.begin()->second[(unsigned)i] << std::endl;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const Stacktrace& s) {
	return s.merged(os);
}

std::ostream& printStacktrace(std::ostream& os) {
	Stacktrace s;
	s.storeTrace("");
	return s.all(os);
}

#ifdef DEBUG
std::string last_assertion_string = "";
int last_assertion_code = 23;

/**
 * Actual signal handler.
 */
void handle_signal(int signal) {
	std::cerr << std::endl << "Catched SIGABRT " << signal << ", exiting with " << (last_assertion_code%128) << std::endl;
	if (last_assertion_string.size() != 0) {
		std::cerr << "Last Assertion catched is: " << last_assertion_string << std::endl;
		std::cerr << "Please check if this is the assertion that is actually thrown." << std::endl;
	}
	exit(last_assertion_code);
}
/**
 * Installs the signal handler.
 */
bool install_signal_handler() {
	std::signal(SIGABRT, handle_signal);
	return true;
}
/**
 * Static variable that ensures that install_signal_handler is called.
 */
static bool signal_installed = install_signal_handler();
#endif

}
