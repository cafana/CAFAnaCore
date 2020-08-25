####  FindOscLib.cmake:
####     Find the Boost library.
####     Assumes you have an $BOOST_DIR and $BOOST_LIB environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(BOOST_DIR $ENV{BOOST_DIR})
message(STATUS "Trying Boost directory: ${BOOST_DIR}")

set(BOOST_VERSION $ENV{BOOST_VERSION})
if(NOT BOOST_VERSION)
	if (EXISTS ${BOOST_DIR}/ups/osclib.table)
		file(READ ${BOOST_DIR}/ups/osclib.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(BOOST_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found Boost version: ${BOOST_VERSION}")

set(BOOST_INC $ENV{BOOST_INC})
if(NOT BOOST_INC)
	if (EXISTS ${BOOST_DIR}/include)
		set(BOOST_INC ${BOOST_DIR}/include)
	endif()
endif()
message(STATUS "Found Boost include dir: ${BOOST_INC}")

# There are many boost libraries
# set(BOOST_LIB $ENV{BOOST_LIB})
# if(BOOST_LIB)
# 	message(STATUS "Found Boost lib dir: ${BOOST_LIB}")
# endif()

# find_library(BOOST_LIBRARY Boost HINTS ${BOOST_LIB})

# function(link_osclib target)
# 	target_link_libraries(${target} PUBLIC ${BOOST_LIBRARY})
# endfunction()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Boost
		REQUIRED_VARS BOOST_DIR BOOST_VERSION BOOST_INC # BOOST_LIB
		VERSION_VAR BOOST_VERSION)
