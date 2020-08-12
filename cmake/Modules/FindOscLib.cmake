####  FindOscLib.cmake:
####     Find the OscLib library.
####     Assumes you have an $OSCLIB_DIR and $OSCLIB_LIB environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(OSCLIB_DIR $ENV{OSCLIB_DIR})
message(STATUS "Trying OscLib directory: ${OSCLIB_DIR}")

set(OSCLIB_VERSION $ENV{OSCLIB_VERSION})
if(NOT OSCLIB_VERSION)
	if (EXISTS ${OSCLIB_DIR}/ups/osclib.table)
		file(READ ${OSCLIB_DIR}/ups/osclib.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(OSCLIB_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found OscLib version: ${OSCLIB_VERSION}")

set(OSCLIB_INC $ENV{OSCLIB_INC})
if(NOT OSCLIB_INC)
	if (EXISTS ${OSCLIB_DIR}/include)
		set(OSCLIB_INC ${OSCLIB_DIR}/include)
	endif()
endif()
message(STATUS "Found OscLib include dir: ${OSCLIB_INC}")

set(OSCLIB_LIB $ENV{OSCLIB_LIB})
if(OSCLIB_LIB)
	message(STATUS "Found OscLib lib dir: ${OSCLIB_LIB}")
endif()

find_library(OSCLIB_LIBRARY OscLib HINTS ${OSCLIB_LIB})

function(link_osclib target)
	target_link_libraries(${target} PUBLIC ${OSCLIB_LIBRARY})
endfunction()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OscLib
		REQUIRED_VARS OSCLIB_DIR OSCLIB_VERSION OSCLIB_INC OSCLIB_LIB
		VERSION_VAR OSCLIB_VERSION)
