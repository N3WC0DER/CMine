cmake_minimum_required(VERSION 3.6.0)
project(CMine++)
include_directories(
	${PROJECT_SOURCE_DIR}/
)
file(GLOB SRCS
	"${PROJECT_SOURCE_DIR}/*"
	"${PROJECT_SOURCE_DIR}/thread/*"
	"${PROJECT_SOURCE_DIR}/utils/*"
	"${PROJECT_SOURCE_DIR}/network/*"
	"${PROJECT_SOURCE_DIR}/network/protocol/*"
	"${PROJECT_SOURCE_DIR}/network/utils/*"
)
add_executable(cmine.out ${SRCS})
