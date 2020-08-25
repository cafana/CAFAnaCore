####  FindEigen.cmake:
####     Find the Eigen library.
####     Assumes you have an $EIGEN_DIR and $EIGEN_LIB environment variables set.
####
####   Original author:  J. Wolcott <jwolcott@fnal.gov>
####   Date:             August 2020

set(EIGEN_DIR $ENV{EIGEN_DIR})
message(STATUS "Trying Eigen directory: ${EIGEN_DIR}")

set(EIGEN_VERSION $ENV{EIGEN_VERSION})
if(NOT EIGEN_VERSION)
	if (EXISTS ${EIGEN_DIR}/ups/osclib.table)
		file(READ ${EIGEN_DIR}/ups/osclib.table table_contents)
		if(${table_contents} MATCHES "VERSION=([^ \t\r\n]+)")
			set(EIGEN_VERSION ${CMAKE_MATCH_1})
		endif()
	endif()
endif()
message(STATUS "Found Eigen version: ${EIGEN_VERSION}")

set(EIGEN_INC $ENV{EIGEN_INC})
if(NOT EIGEN_INC)
	if (EXISTS ${EIGEN_DIR}/include)
		set(EIGEN_INC ${EIGEN_DIR}/include)
	endif()
endif()
message(STATUS "Found Eigen include dir: ${EIGEN_INC}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Eigen
		REQUIRED_VARS EIGEN_DIR EIGEN_VERSION EIGEN_INC
		VERSION_VAR EIGEN_VERSION)
