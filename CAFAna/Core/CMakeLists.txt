set(LIBRARY CAFAnaCoreExt)

file(GLOB HEADER_FILES *.h *.txx)
file(GLOB SOURCES *.cxx)

if(NOT Ifdhc_FOUND)
        list(FILTER HEADER_FILES EXCLUDE REGEX ".*SAM.*")
        list(FILTER SOURCES EXCLUDE REGEX ".*SAM.*")
endif()

add_library(${LIBRARY} SHARED
        ${HEADER_FILES}
        ${SOURCES}
)

link_root(${LIBRARY})
if(Ifdhc_FOUND)
    link_ifdhc(${LIBRARY})
endif()
if(CAFANACORE_USE_STAN)
    link_tbb(${LIBRARY})
endif()

install(TARGETS ${LIBRARY} LIBRARY DESTINATION ${TARGET_LIBDIR})
install_headers("${HEADER_FILES}")
