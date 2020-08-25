####  FindStan.cmake:
####     Find the Stan library.
####     Assumes you have an $STAN_INC and $STAN_MATH_INC environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(STAN_DIR $ENV{STAN_DIR})
message(STATUS "Trying Stan directory: ${STAN_DIR}")

set(STAN_VERSION $ENV{STAN_VERSION})
if(NOT STAN_VERSION)
	if (EXISTS ${STAN_DIR}/ups/stan.table)
		file(READ ${STAN_DIR}/ups/stan.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(STAN_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found Stan version: ${STAN_VERSION}")

set(STAN_INC $ENV{STAN_INC})
set(STAN_MATH_INC $ENV{STAN_MATH_INC})
if(NOT STAN_INC)
	if (EXISTS ${STAN_DIR}/include)
		set(STAN_INC ${STAN_DIR}/include)
	endif()
endif()
message(STATUS "Found Stan include dir: ${STAN_INC}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Stan
		REQUIRED_VARS STAN_DIR STAN_VERSION STAN_INC STAN_MATH_INC
		VERSION_VAR STAN_VERSION)
