cmake_minimum_required(VERSION 3.6.0)
project(simple-threads-docs)

find_package(Doxygen)

if (Doxygen_FOUND)
    if (NOT STH_DOXYGEN_LANGUAGE)
        set(STH_DOXYGEN_LANGUAGE English)
    endif()
    set(DOXYGEN_INPUT ${CMAKE_SOURCE_DIR})
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR})

    set(DOXYGEN_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)
    set(DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    configure_file(${DOXYGEN_CONFIG} ${DOXYFILE} @ONLY)
    message(STATUS "Doxygen documentation will be generated in ${STH_DOXYGEN_LANGUAGE}")

    add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
            WORKING_DIRECTORY ${DOXYGEN_OUTPUT}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else ()
    message(WARNING "Doxygen not found")
endif ()