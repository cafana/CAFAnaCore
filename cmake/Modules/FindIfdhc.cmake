####  FindIfdhc.cmake:
####     Find the IFDHC library.
####     Assumes you have an $IFDHC_FQ_DIR and $IFDHC_LIB environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(IFDHC_DIR $ENV{IFDHC_DIR})
message(STATUS "Trying IFHDC directory: ${IFDHC_DIR}")

set(IFDHC_VERSION $ENV{IFDHC_VERSION})
if(NOT IFDHC_VERSION)
	if (EXISTS ${IFDHC_DIR}/ups/ifdhc.table)
		file(READ ${IFDHC_DIR}/ups/ifdhc.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(IFDHC_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found IFDHC version: ${IFDHC_VERSION}")

set(IFDHC_INC $ENV{IFDHC_FQ_DIR}/inc)
message(STATUS "Found IFDHC include dir: ${IFDHC_INC}")

set(IFDHC_LIB $ENV{IFDHC_LIB})
if(IFDHC_LIB)
	message(STATUS "Found IFDHC lib dir: ${IFDHC_LIB}")
endif()

find_library(IFDHC_LIBRARY ifdh HINTS ${IFDHC_LIB})

function(link_ifdhc target)
	target_link_libraries(${target} PUBLIC ${IFDHC_LIBRARY})
endfunction()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ifdhc
		REQUIRED_VARS IFDHC_DIR IFDHC_VERSION IFDHC_INC IFDHC_LIB
		VERSION_VAR IFDHC_VERSION)
