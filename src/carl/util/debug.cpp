#include "debug.h"

#include "platform.h"
#include "../core/logging.h"

#include <sstream>

namespace carl {

void printStacktrace(bool interaction) {
	std::stringstream cmd;
	cmd << "gdb --pid=" << getpid() << " -ex bt";
	if (!interaction) cmd << " --batch --quiet";
	system(cmd.str().c_str());
}

std::string last_assertion_string = "";
int last_assertion_code = 23;

#ifdef DEBUG
/**
 * Actual signal handler.
 */
void handle_signal(int signal) {
	printStacktrace(false);
	std::cerr << std::endl << "Catched SIGABRT " << signal << ", exiting with " << (last_assertion_code%256) << std::endl;
	if (last_assertion_string.size() != 0) {
		std::cerr << "Last Assertion catched is: " << last_assertion_string << std::endl;
		std::cerr << "Please check if this is the assertion that is actually thrown." << std::endl;
	}
	exit(last_assertion_code % 256);
}
/**
 * Installs the signal handler.
 */
bool install_signal_handler() {
	//LOGMSG_INFO("carl.util", "Installing signal handler for SIGABRT");
	std::signal(SIGABRT, handle_signal);
	return true;
}
/**
 * Static variable that ensures that install_signal_handler is called.
 */
static bool signal_installed = install_signal_handler();
#endif

}
