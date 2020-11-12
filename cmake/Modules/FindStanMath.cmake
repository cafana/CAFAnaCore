####  FindStan.cmake:
####     Find the Stan library.
####     Assumes you have an $STAN_INC and $STAN_MATH_INC environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(STAN_MATH_DIR $ENV{STAN_MATH_DIR})
message(STATUS "Trying StanMath directory: ${STAN_MATH_DIR}")

set(STAN_MATH_VERSION $ENV{STAN_MATH_VERSION})
if(NOT STAN_MATH_VERSION)
	if (EXISTS ${STAN_MATH_DIR}/ups/stan_math.table)
		file(READ ${STAN_MATH_DIR}/ups/stan_math.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(STAN_MATH_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found StanMath version: ${STAN_MATH_VERSION}")

set(STAN_MATH_INC $ENV{STAN_MATH_INC})
if(NOT STAN_MATH_INC)
	if (EXISTS ${STAN_MATH_DIR}/include)
		set(STAN_MATH_INC ${STAN_MATH_DIR}/include)
	endif()
endif()
message(STATUS "Found Stan include dir: ${STAN_MATH_INC}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Stan
		REQUIRED_VARS STAN_MATH_DIR STAN_MATH_VERSION STAN_MATH_INC
		VERSION_VAR STAN_MATH_VERSION)
