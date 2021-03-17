####  FindTBB.cmake:
####     Find the TBB library.
####     Assumes you have an $TBB_DIR and $TBB_LIB environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(TBB_DIR $ENV{TBB_DIR})
message(STATUS "Trying TBB directory: ${TBB_DIR}")

set(TBB_VERSION $ENV{TBB_UPS_VERSION})
if(NOT TBB_VERSION)
	if (EXISTS ${TBB_DIR}/ups/sundials.table)
		file(READ ${TBB_DIR}/ups/sundials.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(TBB_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found TBB version: ${TBB_VERSION}")

set(TBB_INC $ENV{TBB_INC})
if(NOT TBB_INC)
	if (EXISTS ${TBB_DIR}/include)
		set(TBB_INC ${TBB_DIR}/include)
	endif()
endif()
message(STATUS "Found TBB include dir: ${TBB_INC}")

set(TBB_LIB $ENV{TBB_LIB})
if(NOT TBB_LIB)
	if (EXISTS ${TBB_DIR}/lib)
		set(TBB_LIB ${TBB_DIR}/lib)
	endif()
endif()
message(STATUS "Found TBB include dir: ${TBB_INC}")

find_library(TBB_LIBRARY tbb HINTS ${TBB_LIB})

function(link_tbb target)
	target_link_libraries(${target} PUBLIC ${TBB_LIBRARY})
endfunction()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TBB
		REQUIRED_VARS TBB_DIR TBB_VERSION TBB_INC TBB_LIB
		VERSION_VAR TBB_VERSION)
