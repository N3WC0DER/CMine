cmake_minimum_required(VERSION 3.6.0)
project(CMine++)
include_directories(
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/utils/
	${PROJECT_SOURCE_DIR}/network/
)
file(GLOB SRCS
	"${PROJECT_SOURCE_DIR}/*"
	"${PROJECT_SOURCE_DIR}/utils/*"
	"${PROJECT_SOURCE_DIR}/network/*"
)
add_executable(cmine ${SRCS})
