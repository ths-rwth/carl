Logging {#logging}
==================

## Logging frontend

The frontend for logging is defined in [logging.h](@ref logging.h).

It provides the following macros for logging:
- LOGMSG_TRACE(channel, msg)
- LOGMSG_DEBUG(channel, msg)
- LOGMSG_INFO(channel, msg)
- LOGMSG_WARN(channel, msg)
- LOGMSG_ERROR(channel, msg)
- LOGMSG_FATAL(channel, msg)
- LOG_FUNC(channel, args)
- LOG_FUNC(channel, args, msg)
- LOG_ASSERT(channel, condition, msg)
- LOG_NOTIMPLEMENTED()
- LOG_INEFFICIENT()

Where the arguments mean the following:
- `channel`: A string describing the context. For example `"carl.core"`.
- `msg`: The actual message as an expression that can be sent to a std::stringstream. For example `"foo: " << foo`.
- `args`: A description of the function arguments as an expression like `msg`.
- `condition`: A boolean expression that can be passed to `assert()`.

Typically, logging looks like this:
@code{.cpp}
bool checkStuff(Object o, bool flag) {
	LOG_FUNC("carl", o << ", " << flag);
	bool result = o.property(flag);
	LOGMSG_TRACE("carl", "Result: " << result);
	return result;
}
@endcode

Logging is enabled (or disabled) by the `LOGGING` macro in CMake.

## Logging configuration

As of now, there is no frontend interface to configure logging.
Hence, configuration is performed directly on the backend.

## Logging backends

As of now, only two logging backends exist.

### CArL logging

CArL provides a custom logging mechanism defined in carl::logging.

### Fallback logging

If logging is enabled, but no real logging backend is selected, all logging of level `WARN` or above goes to `std::cerr`.
