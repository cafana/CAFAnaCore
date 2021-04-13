set(LIBRARY CAFAnaCoreExt)

file(GLOB HEADER_FILES *.h *.txx)
file(GLOB SOURCES *.cxx)

add_library(${LIBRARY} SHARED
        ${HEADER_FILES}
        ${SOURCES}
)

link_root(${LIBRARY})
link_ifdhc(${LIBRARY})
link_tbb(${LIBRARY})

install(TARGETS ${LIBRARY} LIBRARY DESTINATION ${TARGET_LIBDIR})
install_headers("${HEADER_FILES}")
