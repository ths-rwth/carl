#pragma once

#include <fstream>
#include <ostream>

namespace carl::logging {

/**
 * Base class for a logging sink. It only provides an interface to access some std::ostream.
 */
class Sink {
public:
	/**
	 * Abstract logging interface.
	 * The intended usage is to write any log output to the output stream returned by this function.
	 * @return Output stream.
	 */
	virtual std::ostream& log() noexcept = 0;
};
/**
 * Logging sink that wraps an arbitrary `std::ostream`.
 * It is meant to be used for streams like `std::cout` or `std::cerr`.
 */
class StreamSink final: public Sink {
	/// Output stream.
	std::ostream os;
public:
	/**
	 * Create a StreamSink from some output stream.
	 * @param _os Output stream.
	 */
	explicit StreamSink(std::ostream& _os): os(_os.rdbuf()) {}
	std::ostream& log() noexcept override { return os; }
};
/**
 * Logging sink for file output.
 */
class FileSink: public Sink {
	/// File output stream.
	std::ofstream os;
public:
	virtual ~FileSink() = default;
	/**
	 * Create a FileSink that logs to the specified file.
	 * The file is truncated upon construction.
	 * @param filename
	 */
	explicit FileSink(const std::string& filename): os(filename, std::ios::out) {}
	std::ostream& log() noexcept override { return os; }
};

}