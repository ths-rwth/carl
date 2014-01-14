#include "platform.h"

#if defined(__CLANG) || defined(__GCC)
	#include <execinfo.h>
	#include <dlfcn.h>
	#include <cxxabi.h>
	#include <stdlib.h>
#include <unistd.h>
	std::ostream& printStacktrace(std::ostream& os) {
		os << "stack trace" << std::endl;
		void* stack[128];
		const long unsigned maxFrames = sizeof(stack) / sizeof(void*);
			
		int frames = backtrace(stack, maxFrames);
		char** symbols = backtrace_symbols(stack, frames);
		
		if (frames == 0) {
			return os << "\t<empty, probably corrupt>" << std::endl;
		}

		for (int i = 1; i < frames; i++) {
			os << "\033[31m" << "#" << i << "\033[39m" << "\t" << symbols[i] << std::endl;
			Dl_info info;
			if (dladdr(stack[i], &info) && info.dli_sname) {
				char* demangled = nullptr;
				int status = -1;
				if (info.dli_sname[0] == '_') {
					demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
				}
				os << "\t" << "\033[32m" << (status == 0 ? demangled : info.dli_sname == 0 ? symbols[i] : info.dli_sname)  << " + " << ((char*)stack[i] - (char*)info.dli_saddr);
				free(demangled);
			} else {
				os << "\t" << "\033[32m" << symbols[i] << "\033[39m";
			}
			os << std::endl;
		}
		
		free(symbols);
		if (frames == maxFrames) os << "[truncated]" << std::endl;
		return os;
	}
#else
	std::ostream& printStacktrace(std::ostream& os) {
		return os << "Printing backtrace is not supported for your compiler.";
	}
#endif
