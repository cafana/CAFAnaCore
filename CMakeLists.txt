############   top level config
cmake_minimum_required(VERSION 3.14)
project(CAFAna)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

# default to 'release' build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# make Stan dependency optional, but use it by default
set(CAFANACORE_USE_STAN True CACHE BOOL "Build against Stan?")
if(CAFANACORE_USE_STAN)
    add_compile_definitions(CAFANACORE_USE_STAN)
endif()

# try to get ahold of unifdef, which we'll use to strip all the #ifdef CAFANACORE_USE_STANs out of the packaged include files
include(ExternalProject)
set(UNIFDEF_PREFIX ${PROJECT_BINARY_DIR}/extern-unifdef)
set(UNIFDEF_BIN ${UNIFDEF_PREFIX}/bin) # this is hard-coded in the unifdef makefile
message(STATUS "unifdef will be installed to: ${UNIFDEF_BIN}")
ExternalProject_Add(unifdef
    GIT_REPOSITORY https://github.com/fanf2/unifdef
    GIT_TAG unifdef-2.12
    # do not attempt to use backslashes in regexes in the PATCH_COMMAND, or your head is likely to explode
    PATCH_COMMAND sed -i -e "s|.*prefix =.*|prefix=./|g" -e "s|ln -s .*||g"  <SOURCE_DIR>/Makefile
    CONFIGURE_COMMAND ""
    INSTALL_DIR ${UNIFDEF_PREFIX}
    BUILD_COMMAND make DESTDIR=${UNIFDEF_PREFIX}/ install
    BUILD_IN_SOURCE TRUE
)

# Bake version number into the library (CAFAna/Core/Version.cxx)
execute_process(COMMAND git describe --tags OUTPUT_VARIABLE CAFANACORE_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCAFANACORE_VERSION=\\\"${CAFANACORE_VERSION}\\\"")

option (SPACK "Building through Spack")

# non-negotiable dependencies
if (SPACK)
    find_package(ROOT REQUIRED CONFIG)
    find_package(Eigen3 REQUIRED CONFIG)

    if (CAFANACORE_USE_STAN)
        find_package(Boost REQUIRED CONFIG)
        find_package(SUNDIALS REQUIRED CONFIG)
        find_package(TBB REQUIRED CONFIG)
    endif()

    function(link_tbb target)
        target_link_libraries(${target} PUBLIC TBB::tbb)
    endfunction()
else()
    find_package(ROOT REQUIRED CONFIG)
    find_package(Eigen REQUIRED MODULE)

    if (CAFANACORE_USE_STAN)
        find_package(Boost REQUIRED MODULE)
        find_package(Sundials REQUIRED MODULE)
        find_package(TBB REQUIRED MODULE)
    endif()
endif()

if (CAFANACORE_USE_STAN)
    find_package(StanMath REQUIRED MODULE)
endif()

# pthread / std::thread
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

if(NOT DEFINED NO_IFDHC OR NOT NO_IFDHC)
    find_package(Ifdhc REQUIRED)
else()
    set(Ifdhc_FOUND FALSE)
endif()

# Record what flags were used. Can dump with
# readelf -p .GCC.command.line libCAFAnaCoreExt.so 
set(common_flags "-fdiagnostics-color -grecord-gcc-switches -D_REENTRANT -Wall -Wextra")

# If we can't find "2019" in the version, ie it's newer than that
if(TBB_VERSION AND NOT TBB_VERSION MATCHES 2019)
    set(common_flags "${common_flags} -DTBB_INTERFACE_NEW")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -Og ${common_flags}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${common_flags} -Os")
# notes for release:
# -O3 yields linker errors in some versions of gcc.
#   it's also apparently unstable across some architectures anyway,
#    and apparently doesn't often offer much in the way of performance increases.
#    so let's not cause ourselves headaches
# NDEBUG needed to ensure assert() causes an abort() even in optimized builds
# boost is noisy pointing out deprecated declarations
set(CMAKE_CXX_FLAGS_RELEASE "${common_flags} -O2 -DNDEBUG -g -Wno-deprecated-declarations")

if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}  -Wl,-undefined,error")
elseif()
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")   
endif()

# ROOT does things differently depending on whether it's built for C++17 or not :-\
string(REGEX REPLACE ".*c\\+\\+([0-9][0-9]).*" "\\1" ROOT_CXX_STANDARD ${ROOT_CXX_FLAGS})
if(ROOT_CXX_STANDARD LESS_EQUAL 14)
    message(WARNING "ROOT was built with C++${ROOT_CXX_STANDARD} standard.  Overriding default C++ standard version (C++${CMAKE_CXX_STANDARD}) to use C++14 (lowest supported by this package).  Cross your fingers it builds...")
    set(CMAKE_CXX_STANDARD 14)
endif()

# this is quite lazy -- it links ALL the ROOT libraries into the designated one.
# in principle we should figure out which ones we actually need and link only those.
# but.... some other time
function(link_root target)
    unset(libs CACHE)
    list(APPEND libs ${ROOT_LIBRARIES})
    foreach(lib ${libs})
        target_link_libraries(${target} PUBLIC ${lib})
    endforeach()
endfunction()

# we want to keep the subdirectory structure for the headers.
# all this unifdef stuff replaces the #ifdef CAFANACORE_USE_STAN blocks
# with either the #ifdef or the #else block as appropriate
set(unifdef_var_flag CAFANACORE_USE_STAN)
if(CAFANACORE_USE_STAN)
    set(unifdef_var_flag -iD${unifdef_var_flag})    # assume CAFANACORE_USE_STAN is *defined*
else()
    set(unifdef_var_flag -iU${unifdef_var_flag})    # assume CAFANACORE_USE_STAN is *undefined*
endif()
function(install_headers headers)
    foreach(header ${headers})
        string(REGEX REPLACE "${CMAKE_SOURCE_DIR}/(.*)/[^/]*" "\\1" dir ${header})
        get_filename_component(basename ${header} NAME)
        set(output ${PROJECT_BINARY_DIR}/stripped-headers/${dir}/${basename})
        add_custom_command(OUTPUT ${output}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           COMMAND mkdir -p ${PROJECT_BINARY_DIR}/stripped-headers/${dir}
                           # the 'x' argument makes it only exit with nonzero code in case of error
                           COMMAND "${UNIFDEF_BIN}/unifdef" -x 2 ${unifdef_var_flag} -o ${output} ${header}
                DEPENDS unifdef)
        set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
        add_custom_target(${basename} ALL DEPENDS ${output})
        message(STATUS "using ${UNIFDEF_BIN}/unifdef to convert header ${header} to ${output}")
        install(FILES ${output} DESTINATION inc/${dir})
    endforeach(header ${headers})
endfunction()


set(TARGET_LIBDIR lib)
set(TARGET_BINDIR bin)

###########   directory configuration

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${ROOT_INCLUDE_DIRS}
)
# this is clunky, but Stan-Math needs to be included before Eigen,
# because it adds some type traits to Eigen templates for Stan types
if (CAFANACORE_USE_STAN)
    include_directories(${STAN_MATH_INC})
endif()
include_directories(
        ${EIGEN_INC}
        ${BOOST_INC}
	${SUNDIALS_INC}
        ${TBB_INC}
)

if(Ifdhc_FOUND)
    include_directories(${IFDHC_INC})
endif()

# by default make the bin/ and lib/ dirs in the top of the source dir.
# can by overridden on command line if desired
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "..." FORCE)
endif()
message(STATUS "Installing into: ${CMAKE_INSTALL_PREFIX}")

###########   now go into the subdirs and do the actual work
add_subdirectory(CAFAna)
