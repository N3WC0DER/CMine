cmake_minimum_required(VERSION 3.6.0)
project(cmine
		LANGUAGES CXX C
		VERSION 0.0.1
		DESCRIPTION "Server for Minecraft: PE")

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
			FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
	)
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")

option(CMINE_GENERATE_DOCS "Generate ${PROJECT_NAME} docs" OFF)
option(CMINE_BUILD_TESTS "Enable tests for ${PROJECT_NAME}" OFF)
option(WINDOWS_INCLUDE_WS2_32 "Include ws2_32.lib (For Windows)" OFF)

file(GLOB SRCS
		"${PROJECT_SOURCE_DIR}/src/*"
		"${PROJECT_SOURCE_DIR}/src/thread/*"
		"${PROJECT_SOURCE_DIR}/src/utils/*"
		"${PROJECT_SOURCE_DIR}/src/network/*"
		"${PROJECT_SOURCE_DIR}/src/network/protocol/*"
		"${PROJECT_SOURCE_DIR}/src/network/utils/*")

add_executable(${PROJECT_NAME} ${SRCS})
add_executable(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if (WINDOWS_INCLUDE_WS2_32)
	target_link_libraries(${PROJECT_NAME} C:/Windows/System32/ws2_32.dll)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}
		PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>")

if (CMINE_GENERATE_DOCS)
	add_subdirectory(doc)
endif ()
if (CMINE_BUILD_TESTS)
	message(STATUS "Test not implemented")
endif ()