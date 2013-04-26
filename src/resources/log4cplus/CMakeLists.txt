project(log4cplus)

#TODO Sebastian remove unnecessary options.
set (log4cplus_version_major 1)
set (log4cplus_version_minor 1)
set (log4cplus_version_patch 0)
set (log4cplus_soversion 0)
set (log4cplus_postfix "")


find_package (Threads)
message (STATUS "Threads: ${CMAKE_THREAD_LIBS_INIT}")

# we always build a static library in order to easy the linking in other projects
# set(BUILD_SHARED_LIBS NOT ${STATICLIB_SWITCH})
set(BUILD_SHARED_LIBS OFF)

set (LOG4CPLUS_WORKING_LOCALE_DEFAULT OFF)

set(LOG4CPLUS_WORKING_LOCALE  ${LOG4CPLUS_WORKING_LOCALE_DEFAULT})

set(LOG4CPLUS_WORKING_C_LOCALE OFF)

set(UNICODE OFF)

set(WITH_ICONV  OFF)

if (UNICODE)
  set (log4cplus_postfix "${log4cplus_postfix}U")
endif (UNICODE)

if (WITH_ICONV)
  set(LOG4CPLUS_WITH_ICONV 1)
endif ()

set (log4cplus "log4cplus${log4cplus_postfix}")

include(ConfigureChecks.cmake)

configure_file(include/log4cplus/config.h.cmake.in
  ${log4cplus_BINARY_DIR}/include/log4cplus/config.h @ONLY)
configure_file(include/log4cplus/config/defines.hxx.cmake.in
  ${log4cplus_BINARY_DIR}/include/log4cplus/config/defines.hxx @ONLY)

include_directories (${log4cplus_SOURCE_DIR}/include
                     ${log4cplus_BINARY_DIR}/include
                    )


add_subdirectory (src)
#add_subdirectory (tests)
#add_subdirectory (loggingserver)

include(Log4CPlusCPack.cmake)
