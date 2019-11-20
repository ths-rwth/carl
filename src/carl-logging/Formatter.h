#pragma once

#include "Filter.h"
#include "LogLevel.h"

#include <iomanip>
#include <iosfwd>

namespace carl::logging {

/**
 * Formats a log messages.
 */
class Formatter {
	/// Width of the longest channel.
	std::size_t channelwidth = 10;
public:
	/// Print information like log level, file etc.
	bool printInformation = true;

	virtual ~Formatter() noexcept = default;

	/**
	 * Extracts the maximum width of a channel to optimize the formatting.
	 * @param f Filter.
	 */
	virtual void configure(const Filter& f) noexcept {
		for (const auto& t: f.data()) {
			if (t.first.size() > channelwidth) channelwidth = t.first.size();
		}
	}
	/**
	 * Prints the prefix of a log message, i.e. everything that goes before the message given by the user, to the output stream.
	 * @param os Output stream.
	 * @param channel Channel name.
	 * @param level LogLevel.
	 * @param info Auxiliary information.
	 */
	virtual void prefix(std::ostream& os, const std::string& channel, LogLevel level, const RecordInfo& info) {
		if (!printInformation) return;
		os.fill(' ');
#ifdef THREAD_SAFE
		os << std::this_thread::get_id() << " ";
#endif
		os << level << " ";

		std::string filename(carl::basename(info.filename));
		std::size_t spacing = 1;
		if (channelwidth + 15 > channel.size() + filename.size()) {
			spacing = channelwidth + 15 - channel.size() - filename.size();
		}
		os << channel << std::string(spacing, ' ') << filename << ":" << std::left << std::setw(4) << info.line << " ";
		os << std::resetiosflags(std::ios::adjustfield);
		if (!info.func.empty()) {
			os << info.func << "(): ";
		}
	}

	/**
	 * Prints the suffix of a log message, i.e. everything that goes after the message given by the user, to the output stream.
	 * Usually, this is only a newline.
	 * @param os Output stream.
	 */
	virtual void suffix(std::ostream& os) {
		os << std::endl;
	}
};

}
