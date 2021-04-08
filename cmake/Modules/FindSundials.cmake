####  FindSundials.cmake:
####     Find the SUNDIALS library.
####     Assumes you have an $SUNDIALS_DIR and $SUNDIALS_LIB environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(SUNDIALS_DIR $ENV{SUNDIALS_DIR})
message(STATUS "Trying Sundials directory: ${SUNDIALS_DIR}")

set(SUNDIALS_VERSION $ENV{SUNDIALS_VERSION})
if(NOT SUNDIALS_VERSION)
	if (EXISTS ${SUNDIALS_DIR}/ups/sundials.table)
		file(READ ${SUNDIALS_DIR}/ups/sundials.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(SUNDIALS_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found Sundials version: ${SUNDIALS_VERSION}")

set(SUNDIALS_INC $ENV{SUNDIALS_INC})
if(NOT SUNDIALS_INC)
	if (EXISTS ${SUNDIALS_DIR}/include)
		set(SUNDIALS_INC ${SUNDIALS_DIR}/include)
	endif()
endif()
message(STATUS "Found Sundials include dir: ${SUNDIALS_INC}")

set(SUNDIALS_LIB $ENV{SUNDIALS_LIB})
if(NOT SUNDIALS_LIB)
	if (EXISTS ${SUNDIALS_DIR}/lib)
		set(SUNDIALS_LIB ${SUNDIALS_DIR}/lib)
	endif()
endif()
message(STATUS "Found Sundials include dir: ${SUNDIALS_INC}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sundials
		REQUIRED_VARS SUNDIALS_DIR SUNDIALS_VERSION SUNDIALS_INC SUNDIALS_LIB
		VERSION_VAR SUNDIALS_VERSION)
