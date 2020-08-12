####  FindSRProxy.cmake:
####     Find the SRProxy library.
####     Assumes you have an $SRPROXY_DIR environment variable set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(SRPROXY_DIR $ENV{SRPROXY_DIR})
message(STATUS "Trying SRProxy directory: ${SRPROXY_DIR}")

set(SRPROXY_VERSION $ENV{SRPROXY_VERSION})
if(NOT SRPROXY_VERSION)
	if (EXISTS ${SRPROXY_DIR}/ups/srproxy.table)
		file(READ ${SRPROXY_DIR}/ups/srproxy.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(SRPROXY_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found SRProxy version: ${SRPROXY_VERSION}")

set(SRPROXY_INC $ENV{SRPROXY_INC})
if(NOT SRPROXY_INC)
	if (EXISTS ${SRPROXY_DIR}/include)
		set(SRPROXY_INC ${SRPROXY_DIR}/include)
	endif()
endif()
message(STATUS "Found SRProxy include dir: ${SRPROXY_INC}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SRProxy
		REQUIRED_VARS SRPROXY_DIR SRPROXY_VERSION SRPROXY_INC
		VERSION_VAR SRPROXY_VERSION)
