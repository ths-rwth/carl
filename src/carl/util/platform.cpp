#include "platform.h"

#if defined(__CLANG) || defined(__GCC)
	#include <execinfo.h>
	#include <dlfcn.h>
	#include <cxxabi.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <map>
	std::ostream& printStacktrace(std::ostream& os, bool colors) {
		os << "stack trace" << std::endl;
		void* stack[128];
		const long unsigned maxFrames = sizeof(stack) / sizeof(void*);

		std::map<std::string, std::string> col;
		if (colors) {
			col["green"] = "\033[31m";
			col["reset"] = "\033[39m";
			col["red"] = "\033[32m";
		}

		int frames = backtrace(stack, maxFrames);
		char** symbols = backtrace_symbols(stack, frames);
		
		if (frames == 0) {
			return os << "\t<empty, probably corrupt>" << std::endl;
		}

		for (int i = 1; i < frames; i++) {
			os << col["green"] << "#" << i << col["reset"] << "\t" << symbols[i] << std::endl;
			Dl_info info;
			if (dladdr(stack[i], &info) && info.dli_sname) {
				char* demangled = nullptr;
				int status = -1;
				if (info.dli_sname[0] == '_') {
					demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
				}
				os << "\t" << col["red"] << (status == 0 ? demangled : info.dli_sname == 0 ? symbols[i] : info.dli_sname)  << " + " << ((char*)stack[i] - (char*)info.dli_saddr);
				free(demangled);
			} else {
				os << "\t" << col["red"] << symbols[i] << col["reset"];
			}
			os << std::endl;
		}
		
		free(symbols);
		if (frames == maxFrames) os << "[truncated]" << std::endl;
		return os;
	}
#else
	std::ostream& printStacktrace(std::ostream& os, bool colors) {
		return os << "Printing backtrace is not supported for your compiler.";
	}
#endif
